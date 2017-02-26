rule vir_infect_example
{
    meta:
        description = "Non-malicious EXE infection example"
        in_the_wild = false

    strings:
        $a = {47 65 74 45 C7 45 D0 6E 76 69 72 C7 45 D4 6F 6E}
        $b = {C7 85 30 FE FF FF 2E 76 69 72 C7 85 34 FE FF FF 00 00 00 00}

    condition:
        $a and $b
}

