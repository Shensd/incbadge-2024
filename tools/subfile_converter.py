import sys

def main(argc, argv):
    if argc < 2:
        print(f"usage: {argv[0]} capture.sub")
        return

    subfile_path = argv[1]

    with open(subfile_path, "r") as subfile_fd:
        subfile_lines = subfile_fd.readlines()

    frequency = preset = protocol = raw_data = custom_preset_data = None

    for line in subfile_lines:
        tokens = line.split(": ")
        key, value = tokens[0], ": ".join(tokens[1:])

        if key == "Frequency":
            frequency = value.strip()
        if key == "Preset":
            preset = value.strip()
        if key == "Custom_preset_data":
            custom_preset_data = value.strip()
        if key == "Protocol":
            protocol = value.strip()
        if key == "RAW_Data":
            if raw_data == None:
                raw_data = []
            raw_data.extend(value.strip().split(" "))

    MAX_TIMINGS = 4096

    preset_map = {
        "FuriHalSubGhzPresetOok270Async": "radiohal::CONFIG_PRESET_AM270",
        "FuriHalSubGhzPresetOok650Async": "radiohal::CONFIG_PRESET_AM650",
        "FuriHalSubGhzPreset2FSKDev238Async": "radiohal::CONFIG_PRESET_FM238",
        "FuriHalSubGhzPreset2FSKDev476Async": "radiohal::CONFIG_PRESET_FM476",
        "FuriHalSubGhzPresetCustom": "radiohal::CONFIG_PRESET_CUSTOM",
    }

    protcol_map = {
        "RAW": "radiohal::CONFIG_PROTOCOL_RAW",
    }

    if frequency == None:
        print("Sub file missing required key 'Frequency'.")
        return
    if preset == None:
        print("Sub file missing required key 'Preset'.")
        return
    if preset not in preset_map:
        print(f"Preset {preset} is not currently supported.")
        return
    if preset == "FuriHalSubGhzPresetCustom" and custom_preset_data == None:
        print("Sub file uses custom preset but is missing Сustom_preset_data field.")
        return
    if protocol == None:
        print("Sub file missing required key 'Protocol'.")
        return
    if protocol not in protcol_map:
        print(f"Protocol {protocol} is not currently supported.")
        return
    if raw_data == None:
        print("Sub file is required to have at least one RAW_Data field")
        return
    if len(raw_data) == 0:
        print("Sub file has RAW_Data field but it is empty")
        return
    if len(raw_data) > MAX_TIMINGS:
        print(f"Sub file has too many timings, max {MAX_TIMINGS} (sub file has {len(raw_data)})")
    
    print("    {")
    name = subfile_path.split("/")[-1].split(".")[0]
    print(f"        .name = \"{name}\",")
    print(f"        .frequency_hz = {frequency},")
    preset_index = preset_map[preset]
    print(f"        .preset_index = {preset_index},")
    protocol_index = protcol_map[protocol]
    print(f"        .custom_protocol_index = {protocol_index},")
    if custom_preset_data != None:
        custom_preset_string = ", ".join(list(map(lambda x: "0x" + x, custom_preset_data.split(" "))))
        print(f"        .custom_preset_data = {{ {custom_preset_string} }}")
    raw_data_string = ", ".join(raw_data)
    print(f"        .timings = {{\n            {raw_data_string}\n        }},")
    print(f"        .num_timings = {len(raw_data)}")
    print("    },")

if __name__ == "__main__":
    main(len(sys.argv), sys.argv)