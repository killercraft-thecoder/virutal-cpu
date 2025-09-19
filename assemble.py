#!/usr/bin/env python3
import argparse
import os
import re
import sys
import time
from typing import Any,Literal

# ---------------- Instruction set ----------------

OPCODES = {
    'ADD': 0x00,
    'SUB': 0x01,
    'INC': 0x02,
    'DEC': 0x03,
    'B'  : 0x04,
    'BNZ': 0x05,
    'BZ' : 0x06,
    'STX': 0x07,
    'XTS': 0x08,
    'LDA': 0x09,
    'STA': 0x0A,
    'BR' : 0x0B,
    'XTA': 0x0C,
    'ATX': 0x0D,
    'LDX': 0x0E,
    'STX': 0x0F,
    'JSR': 0x10,
    'RTS': 0x11,
    'BSR': 0x12,
    'BN': 0x13,
    'BNR': 0x14,
    'BPR': 0x15,
    'BP':  0x16,
    'BC':  0x17,
    'BCR': 0x18,
    'XSRA': 0x19,
'XSLA': 0x1A,
'ASRX': 0x1B,
'ASLX': 0x1C,
'AND': 0x1D,
'OR':  0x1E,
'XOR': 0x1F,
'EOR': 0x1F,
'CLF': 0x20,
'CLC': 0x21,
'CLN': 0x22,
'CLZ': 0x23,
'XXA': 0x24,
'BRR': 0x25,
'RTR': 0x26,
'BA': 0x27,
'ADDF': 0x28,
'SUBF': 0x29,
'BNC':  0x2A,
'BNCR': 0x2B,
'PHA':  0x2C,
'PLA':  0x2D,
'PHX':  0x2E,
'PLX':  0x2F,
'NOTA': 0x30,
'NOTX': 0x31,
'NEG':  0x32,
'SWAP': 0x33,
'ADC':0x34,
'SBC':0x35,
'SETBRK':0x36,
'BRK':0x37,
'ROL': 0x38,  # Rotate Left (Accumulator)
    'ROR': 0x39,  # Rotate Right (Accumulator)
    'ASL': 0x3A,  # Arithmetic Shift Left (Accumulator)
    'ASR': 0x3B,  # Arithmetic Shift Right (Accumulator)
'BVS': 0x3C,  # absolute
    'BVC': 0x3D,  # absolute
    'BVSR': 0x3E, # relative
    'BVCR': 0x3F, # relative
    'JSRI': 0x40,  # Jump to SubRoutine Indirect
    'BX': 0x41,
    'BAX':0x42,
    'DECOD':0x44,
    'DECBIN':0x45,
    'ADDBCD':0x46,
    'SUBBCD':0x47,
    'LDAD':0x48,
    'LDSUB':0x49,
    'LD2':0x4A,
    'ST2':0x4B,
    'TST':0x4C,




}

SIZES = {
    0x00: 1, 0x01: 1, 0x02: 1, 0x03: 1, 0x07: 1, 0x08: 1, 0x0C: 1, 0x0D: 1,
    0x04: 3, 0x05: 3, 0x06: 3, 0x09: 3, 0x0A: 3,
    0x0B: 2,0x0E: 3,0x0F: 3, 0x10: 3,  # JSR abs16
    0x11: 1,  # RTS implied
    0x12: 2,  # BSR rel8
    0x13: 3,  # BN abs16
    0x14: 2,  # BNR rel8
    0x15: 2,  # BPR rel8
    0x16: 3,  # BP abs16
    0x17: 3,  # BC abs16
    0x18: 2,  # BCR rel8
    0x19: 1,
0x1A: 1,
0x1B: 1,
0x1C: 1,
0x1D: 1,
0x1E: 1,
0x1F: 1,
0x20: 1,
0x21: 1,
0x22: 1,
0x23: 1,
0x24: 1,
0x25: 2,  # BRR rel8
0x26: 1,  # RTR implied
0x27: 1,
0x28: 1,
0x29: 1,
0x2A: 3,  # BNC abs16
0x2B: 2,  # BNCR rel8
0x2C: 1,
0x2D: 1,
0x2E: 1,
0x2F: 1,
0x30: 1,
0x31: 1,
0x32: 1,
0x33: 1,
0x34:2,
0x35:2,
0x36:3,
0x37:1,
0x38: 1,  # ROL A
    0x39: 1,  # ROR A
    0x3A: 1,  # ASL A
    0x3B: 1,  # ASR A
    0x3C: 3,  # BVS abs (opcode + 2-byte address)
    0x3D: 3,  # BVC abs
    0x3E: 2,  # BVS rel (opcode + 1-byte offset)
    0x3F: 2,  # BVC rel
0x40: 3,  # opcode + 16-bit pointer address
0x41: 1,  # opcode only
0x42:1, # opcode only
0x44:1,
0x45:1,
0x46:1,
0x47:1,
0x48:3, # opcode + 16-bit pointer address
0x49:3,
0x4A:3,
0x4B:3,
0x4C:1,
}

# ---------------- Utilities ----------------

def parse_number(s: str) -> int:
    s = s.strip()
    if not s:
        raise ValueError("empty number")
    sl = s.lower()
    if sl.startswith('$'):
        return int(sl[1:], 16)
    if sl.startswith('0x'):
        return int(sl, 16)
    if sl.startswith('%'):
        return int(sl[1:], 2)
    if re.fullmatch(r'[0-9a-f]+h', sl):
        return int(sl[:-1], 16)
    if sl.startswith("'") and sl.endswith("'") and len(sl) == 3:
        return ord(sl[1])
    return int(sl, 10)

def strip_comment(line: str) -> str:
    for sep in (';', '//'):
        i = line.find(sep)
        if i != -1:
            line = line[:i]
    return line.rstrip()

def tokenize_operands(s: str):
    return [t for t in re.split(r'[,\s]+', s.strip()) if t]

def checksum16(bs: bytes) -> int:
    return sum(bs) & 0xFFFF

# ---------------- Assembler ----------------

class Assembler:
    def __init__(self, origin=0x0000, fill=0x00, include_paths=None, cli_defines=None) -> None:
        self.origin = origin & 0xFFFF
        self.fill = fill & 0xFF
        self.include_paths = include_paths or []
        self.labels = {}
        self.defines = dict(cli_defines or {})
        self.lines = []    # list of (file, line_no, text)
        self.segments = [] # list of (addr, bytes)
        self.errors = []

    # ---------- Preprocessing and loading ----------

    def resolve_include(self, path, current_file) -> Any | None:
        # Absolute
        if os.path.isabs(path) and os.path.exists(path):
            return path
        # Relative to current file
        if current_file:
            base = os.path.dirname(os.path.abspath(current_file))
            rel = os.path.join(base, path)
            if os.path.exists(rel):
                return rel
        # -I include paths
        for inc in self.include_paths:
            cand = os.path.join(inc, path)
            if os.path.exists(cand):
                return cand
        return None

    def load_file(self, filename, _seen=None) -> None:
        filename = os.path.abspath(filename)
        if _seen is None:
            _seen = set()
        if filename in _seen:
            self.errors.append(f"Circular include detected: {filename}")
            return
        _seen.add(filename)
        try:
            with open(filename, 'r', encoding='utf-8') as fh:
                raw_lines = fh.readlines()
        except OSError as e:
            self.errors.append(f"Cannot open {filename}: {e}")
            return

        for ln, raw in enumerate(raw_lines, 1):
            line = strip_comment(raw)
            if not line.strip():
                continue

            # Includes: %include "file" or include "file"
            inc_m = re.match(r'^\s*(%include|include)\s+(.+)$', line, re.IGNORECASE)
            if inc_m:
                rest = inc_m.group(2).strip()
                m = re.match(r'^"(.*)"$', rest)
                if m:
                    inc_path = m.group(1)
                else:
                    inc_path = rest.split()[0]
                resolved = self.resolve_include(inc_path, filename)
                if not resolved:
                    self.errors.append(f"{filename}:{ln}: include not found: {inc_path}")
                else:
                    self.load_file(resolved, _seen=_seen)
                continue

            # #define NAME VALUE
            def_m = re.match(r'^\s*#define\s+([A-Za-z_]\w*)\s+(.+)$', line)
            if def_m:
                name = def_m.group(1)
                val = def_m.group(2).strip()
                self.defines[name] = val
                continue

            # .equ NAME VALUE   OR   .equ NAME, VALUE
            equ_m = re.match(r'^\s*\.equ\s+([A-Za-z_]\w*)\s*(?:,|\s)\s*(.+)$', line, re.IGNORECASE)
            if equ_m:
                name = equ_m.group(1)
                val = equ_m.group(2).strip()
                self.defines[name] = val
                continue

            self.lines.append((filename, ln, line))

    # ---------- Pass 1: label addresses ----------

    def pass1(self) -> None:
        pc = self.origin
        for file, ln, line in self.lines:
            cur = line.strip()
            # Handle labels (possibly multiple)
            while True:
                m = re.match(r'^([A-Za-z_]\w*):\s*(.*)$', cur)
                if not m:
                    break
                label, rest = m.group(1), m.group(2)
                if label in self.labels:
                    self.errors.append(f"{file}:{ln}: duplicate label '{label}'")
                else:
                    self.labels[label] = pc
                cur = rest.strip()
                if not cur:
                    break
            if not cur:
                continue

            # .org directive
            if cur.lower().startswith('.org'):
                parts = tokenize_operands(cur)
                if len(parts) != 2:
                    self.errors.append(f"{file}:{ln}: .org requires one address")
                    continue
                try:
                    pc = self.eval_token(parts[1])
                except Exception:
                    self.errors.append(f"{file}:{ln}: invalid .org address '{parts[1]}'")
                continue

            parts = tokenize_operands(cur)
            mnem = parts[0].upper()
            op = OPCODES.get(mnem)
            if op is None:
                self.errors.append(f"{file}:{ln}: unknown mnemonic '{mnem}'")
                continue
            pc += SIZES[op]

        if self.errors:
            raise SystemExit("\n".join(self.errors))

    # ---------- Pass 2: encode ----------

    def eval_token(self, tok: str) -> int:
        # Replace chained defines up to a small limit
        seen = set()
        val = tok
        for _ in range(16):
            if val in self.defines and val not in seen:
                seen.add(val)
                val = self.defines[val]
            else:
                break
        # Labels win over numbers if exact match
        if val in self.labels:
            return self.labels[val]
        return parse_number(val)

    def pass2(self) -> None:
        pc = self.origin
        out = []
        seg_base = None

        def start_segment_if_needed():
            nonlocal seg_base
            if seg_base is None:
                seg_base = pc

        def emit(b:int) -> None:
            if (b): out.append(b & 0xFF)

        for file, ln, line in self.lines:
            cur = line.strip()
            # Skip labels on this line
            while True:
                m = re.match(r'^([A-Za-z_]\w*):\s*(.*)$', cur)
                if not m:
                    break
                cur = m.group(2).strip()
                if not cur:
                    break
            if not cur:
                continue

            # .org
            if cur.lower().startswith('.org'):
                if out and seg_base is not None:
                    self.segments.append((seg_base, bytes(out)))
                    out = []
                parts = tokenize_operands(cur)
                pc = self.eval_token(parts[1])
                seg_base = None
                continue

            parts = tokenize_operands(cur)
            mnem = parts[0].upper()
            op = OPCODES.get(mnem)
            if op is None:
                self.errors.append(f"{file}:{ln}: unknown mnemonic '{mnem}'")
                continue
            size = SIZES[op]
            start_segment_if_needed()
            emit(op)

            if size == 1:
                pc += 1
                continue

            if size == 2:
                # Immediate or zero-page style: single operand byte
                if len(parts) != 2:
                    self.errors.append(f"{file}:{ln}: {mnem} requires 1 operand")
                    emit(0x00)
                    pc += 2
                    continue
                val = self.eval_token(parts[1])
                if val < 0 or val > 0xFF:
                    self.errors.append(f"{file}:{ln}: operand out of range for {mnem}: {val}")
                emit(val)
                pc += 2
                continue


            if op in (0x0B, 0x12, 0x14, 0x15, 0x16, 0x18, 0x2B):  # BR or BSR
                if len(parts) != 2:
                    self.errors.append(f"{file}:{ln}: {mnem} requires 1 operand")
                    emit(0x00)  # placeholder
                    pc += 2
                    continue
                target = self.eval_token(parts[1])
                rel = target - (pc + 2)
                if rel < -128 or rel > 127:
                    self.errors.append(f"{file}:{ln}: ${mnem} target out of range ({rel})")
                emit(rel & 0xFF)
                pc += 2
                continue

            # abs16 ops
            if len(parts) != 2:
                self.errors.append(f"{file}:{ln}: {mnem} requires 1 operand")
                emit(0x00); emit(0x00)
                pc += 3
                continue
            addr = self.eval_token(parts[1])
            if addr < 0 or addr > 0xFFFF:
                self.errors.append(f"{file}:{ln}: address out of range for {mnem}: {addr}")
            emit(addr & 0xFF)
            emit((addr >> 8) & 0xFF)
            pc += 3

        if out and seg_base is not None:
            self.segments.append((seg_base, bytes(out)))

        if self.errors:
            raise SystemExit("\n".join(self.errors))

    # ---------- Image and outputs ----------

    def build_image(self) -> tuple[int, Literal[b""]] | tuple[Any, bytes]:
        if not self.segments:
            return self.origin, b""
        base = min(addr for addr, _ in self.segments)
        end = max(addr + len(data) for addr, data in self.segments)
        img = bytearray([self.fill] * (end - base))
        for addr, data in self.segments:
            off = addr - base
            img[off:off+len(data)] = data
        return base, bytes(img)

    def to_cpp(self, var='ROM', origin=None, cols=12):
        base, img = self.build_image()
        origin = base if origin is None else origin
        lines = []
        lines.append(f"const uint16_t {var}_ORIGIN = 0x{origin:04X};")
        lines.append(f"const uint8_t {var}[] = {{")
        for i in range(0, len(img), cols):
            chunk = img[i:i+cols]
            hexes = ", ".join(f"0x{b:02X}" for b in chunk)
            lines.append(f"  {hexes},")
        if not img:
            lines.append("  // empty")
        lines.append("};")
        return "\n".join(lines)

    def to_rom_bytes(self, origin_override=None) -> bytes:
        base, img = self.build_image()
        origin = base if origin_override is None else (origin_override & 0xFFFF)
        size = len(img) & 0xFFFF
        csum = checksum16(img)
        # Header: magic(4) ver(1) res(1) origin(2) size(2) csum(2)
        hdr = bytearray()
        hdr += b"MR8C"
        hdr += bytes([1, 0])  # version=1, reserved=0
        hdr += bytes([origin & 0xFF, (origin >> 8) & 0xFF])
        hdr += bytes([size & 0xFF, (size >> 8) & 0xFF])
        hdr += bytes([csum & 0xFF, (csum >> 8) & 0xFF])
        return bytes(hdr) + img

# ---------------- CLI ----------------

def main() -> None:
    ap = argparse.ArgumentParser(description="Assembler for the minimal 8-bit CPU")
    ap.add_argument("input", help="Assembly file (entry), use '-' for stdin")
    ap.add_argument("--origin", "-o", default="0x0000", help="Default origin if no .org")
    ap.add_argument("--fill", "-f", default="0x00", help="Fill byte for gaps")
    ap.add_argument("--var", "-v", default="ROM", help="C++ array variable name (cpp output)")
    ap.add_argument("--out-format", "-t", choices=["rom", "cpp"], default="rom",
                    help="Output format: rom binary or cpp source")
    ap.add_argument("--output", "-O", help="Output file path (defaults: stdout for cpp, input with .rom for rom)")
    ap.add_argument("-I", dest="includes", action="append", default=[], help="Add include search path")
    ap.add_argument("-D", dest="defines", action="append", default=[], help="Define NAME=VALUE or NAME")
    args = ap.parse_args()

    def parse_def(d):
        if '=' in d:
            k, v = d.split('=', 1)
            return k.strip(), v.strip()
        return d.strip(), "1"

    cli_defines = dict(parse_def(d) for d in args.defines)
    origin = parse_number(args.origin)
    fill = parse_number(args.fill)

    starttime = time.monotonic()
    asm = Assembler(origin=origin, fill=fill, include_paths=args.includes, cli_defines=cli_defines)

    if args.input == "-":
        # For stdin, write to a temp synthetic "stdin.asm" node
        asm.lines.append(("<stdin>", 1, strip_comment(sys.stdin.read())))
    else:
        asm.load_file(args.input)

    asm.pass1()
    asm.pass2()

    if args.out_format == "cpp":
        text = asm.to_cpp(var=args.var, origin=origin)
        if args.output:
            with open(args.output, "w", encoding="utf-8") as fh:
                fh.write(text)
        else:
            print(text)
    else:
        blob = asm.to_rom_bytes(origin_override=origin)
        out_path = args.output
        if not out_path:
            base, _ = os.path.splitext(os.path.abspath(args.input))
            out_path = base + ".rom"
        with open(out_path, "wb") as fh:
            fh.write(blob)
        endtime = time.monotonic()
        # Print a short note to stderr
        sys.stderr.write(f"Wrote ROM: {out_path} Built ROM in {(endtime-starttime) * 1000} milliseconds.\n")

if __name__ == "__main__":
    main()