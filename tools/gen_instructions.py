import json

FORMAT_NONE             = "exec_instruction([&] () {{ instr_{}(); }}, {})"

FORMAT_SINGLE           = "exec_instruction([&] () {{ instr_{}({}); }}, {})"
FORMAT_SINGLE_BYTE      = "exec_instruction([&] (Byte &v) {{ instr_{}({}); }}, {}, {})"

FORMAT_DOUBLE           = "exec_instruction([&] () {{ instr_{}({}, {}); }}, {})"
FORMAT_DOUBLE_BYTE      = "exec_instruction([&] (Byte &v) {{ instr_{}({}, {}); }}, {}, {})"
FORMAT_DOUBLE_WORD      = "exec_instruction([&] (Word &v) {{ instr_{}({}, {}); }}, {}, {})"

fetch_byte              = "_components.mem_bus->read<Byte>(addr++)"

def get_deref_right(mnemonic, operand1, operand2):
    if "(" in operand1:
        operand = operand1
    elif "(" in operand2:
        operand = operand2
    else:
        # Should not happen
        abort()
        return None

    prefix      = "_components.mem_bus->read<Byte>("
    suffix      = ")"
    operand     = operand[1:-1]
    registers   = {
        "HL": "_hl.word", "DE": "_de.word",
        "C": "0xFF00 + _bc.low", "BC": "_bc.word",
        "a16": "fetch_word()", "a8": "0xFF00 + " + fetch_byte,
        "d8": "0xFF00 + " + fetch_byte, "d16": "fetch_word()"
    }
    return prefix + registers.get(operand) + suffix

def get_deref(mnemonic, operand1, operand2):
    if "(" in operand1:
        operand = operand1
    elif "(" in operand2:
        operand = operand2
    else:
        # Should not happen
        abort()
        return None

    operand     = operand[1:-1]
    registers   = {
        "HL": "_hl.word", "DE": "_de.word",
        "C": "0xFF00 + _bc.low", "BC": "_bc.word",
        "a16": "fetch_word()", "a8": "0xFF00 + " + fetch_byte,
        "d8": "0xFF00 + " + fetch_byte, "d16": "fetch_word()"
    }
    return registers.get(operand)

def get_reg(mnemonic, operand):
    registers = {
        "AF": "_af.word", "BC": "_bc.word",
        "DE": "_de.word", "HL": "_hl.word",
        "A": "_af.high", "F": "_af.low",
        "B": "_bc.high", "C": "_bc.low",
        "D": "_de.high", "E": "_de.low",
        "H": "_hl.high", "L": "_hl.low",
        "SP": "_sp.word",
        "d8": fetch_byte, "d16": "fetch_word()",
        "r8": fetch_byte,
        "SP+r8": fetch_byte
    }

    if mnemonic in ("jr", "jp", "call", "ret"):
        condprefix = "JumpCondition::"
        conditionals = {
            "C": "Carry", "NC": "NonCarry", "Z": "Zero", "NZ": "NonZero",
        }
        condreg = {
            "r8": fetch_byte, "a16": "fetch_word()",
            "HL": "_hl.word"
        }
        cond = conditionals.get(operand)
        if cond is not None:
            return condprefix + cond
        return condreg.get(operand)

    if mnemonic == "rst":
        rst =  {
            "00H": "0x00", "08H": "0x08",
            "10H": "0x10", "18H": "0x18",
            "20H": "0x20", "28H": "0x28",
            "30H": "0x30", "38H": "0x38"
        }
        return rst.get(operand)

    reg = registers.get(operand)
    if reg is None:
        return operand
    return reg

def gen_code(opcodes, cb=False):
    for key, opcode in opcodes.items():
        deref               = None
        addr                = opcode["addr"]
        mnemonic            = opcode["mnemonic"].lower()
        operand1            = opcode.get("operand1")
        operand2            = opcode.get("operand2")
        exec_instruction    = None
        first_is_deref      = False

        cycles              = opcode.get("cycles")

        if len(cycles) > 1:
            cycles = str(cycles[0]) + ", " + str(cycles[1])
        else:
            cycles = str(cycles[0])

        if (operand1 and "(" in operand1) or (operand2 and "(" in operand2):
            deref = get_deref(mnemonic, operand1, operand2)
            if operand1 and "(" in operand1:
                first_is_deref = True

        if mnemonic == "ld" and operand2 == "SP+r8":
            mnemonic = "ldhl"
            operand1 = operand2
            operand2 = None

        cpp_op1 = get_reg(mnemonic, operand1)
        cpp_op2 = get_reg(mnemonic, operand2)

        # Annoying instruction #2 #3 #4 #5 #6
        # Thanks to Alaric's way of handling these instructions the following elif is necessary
        if mnemonic in ("sub", "and", "or", "xor", "cp"):
            cpp_op1 = "_af.high"
            if deref:
                cpp_op2 = get_deref_right(mnemonic, cpp_op1, operand1)
                exec_instruction = FORMAT_DOUBLE.format(mnemonic.lower(), cpp_op1, cpp_op2, cycles)
            else:
                cpp_op2 = get_reg(mnemonic, operand1)
                exec_instruction = FORMAT_DOUBLE.format(mnemonic, cpp_op1, cpp_op2, cycles)
        elif operand1 and operand2:
            _format = None
            if mnemonic in ("bit", "res", "set"):
                if deref:
                    cpp_op2 = get_deref(mnemonic, operand1, operand2)
                    exec_instruction = FORMAT_DOUBLE_BYTE.format(mnemonic, cpp_op1, "v", deref, cycles)
                else:
                    exec_instruction = FORMAT_DOUBLE.format(mnemonic, cpp_op1, cpp_op2, cycles)
            elif deref:
                if operand2 == "SP":
                    _format = FORMAT_DOUBLE_WORD
                else:
                    _format = FORMAT_DOUBLE_BYTE
                if first_is_deref:
                    exec_instruction = _format.format(mnemonic.lower(), "v", cpp_op2, deref, cycles)
                else:
                    cpp_op2 = get_deref_right(mnemonic, operand1, operand2)
                    exec_instruction = FORMAT_DOUBLE.format(mnemonic, cpp_op1, cpp_op2, cycles)
            else:
                exec_instruction = FORMAT_DOUBLE.format(mnemonic, cpp_op1, cpp_op2, cycles)
        elif operand1:
            if deref:
                if mnemonic in ("and", "or", "xor"):
                    cpp_op1 = get_deref_right(mnemonic, operand1)
                    exec_instruction = FORMAT_SINGLE.format(mnemonic, cpp_op1, cycles)
                else:
                    exec_instruction = FORMAT_SINGLE_BYTE.format(mnemonic, "v", deref, cycles)
            else:
                exec_instruction = FORMAT_SINGLE.format(mnemonic, cpp_op1, cycles)
        else:
            exec_instruction = FORMAT_NONE.format(mnemonic, cycles)
        print("case " + addr + ":")
        print("#ifdef DEBUG_CORE")
        if operand1 and operand2:
            print('std::printf("%04x: {} {} {}\\n", _pc.word);'.format(mnemonic, operand1, operand2))
        elif operand1:
            print('std::printf("%04x: {} {}\\n", _pc.word);'.format(mnemonic, operand1, operand2))
        else:
            print('std::printf("%04x: {}\\n", _pc.word);'.format(mnemonic))
        print("#endif")
        print("  " + "_current_opcode = " + addr + ";");
        print("  " + exec_instruction + ";")
        print("  break;")

def main():
    opcodes = json.loads(open("opcodes.json", "r").read())
    del opcodes["unprefixed"]["0xcb"]
    gen_code(opcodes.get("unprefixed"))
    print("  case 0xCB:")
    print("switch (" + fetch_byte + ") {")
    gen_code(opcodes.get("cbprefixed"), cb=True)
    print("}")

if __name__ == "__main__":
    main()
