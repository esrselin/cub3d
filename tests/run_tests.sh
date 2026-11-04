#!/bin/bash
# cub3D test runner.
#   ./tests/run_tests.sh            run bad + good scene tests
#   ./tests/run_tests.sh norm       norminette + Norm-trap heuristics
#   VALGRIND=1 ./tests/run_tests.sh run the bad paths under valgrind too
# Env: CUB=<binary> (default ./cub3D), TIMEOUT=<s> for good maps (default 2)

cd "$(dirname "$0")/.." || exit 1
BIN=${CUB:-./cub3D}
TMO=${TIMEOUT:-2}
SUPP=valgrind.supp
pass=0
fail=0
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

ok()   { pass=$((pass + 1)); printf '  \033[32mOK\033[0m    %s\n' "$1"; }
ko()   { fail=$((fail + 1)); printf '  \033[31mFAIL\033[0m  %s  -> %s\n' "$1" "$2"; }

# A bad input must: exit non-zero, not die from a signal (code < 128,
# and not 124 = timeout), and print "Error" as the first line.
expect_error() {
	local label=$1; shift
	local out code first
	out=$(timeout "$TMO" "$BIN" "$@" 2>&1); code=$?
	first=$(printf '%s\n' "$out" | head -1)
	if [ "$code" -eq 0 ]; then ko "$label" "exited 0 (accepted bad input)"
	elif [ "$code" -eq 124 ]; then ko "$label" "timeout: window opened on bad input"
	elif [ "$code" -ge 128 ]; then ko "$label" "crashed (signal $((code - 128)))"
	elif [ "$first" != "Error" ]; then ko "$label" "first line is '$first', not 'Error'"
	else ok "$label ($(printf '%s\n' "$out" | sed -n 2p))"; fi
	if [ -n "$VALGRIND" ] && [ "$code" -lt 124 ]; then
		out=$(timeout 30 valgrind -q --leak-check=full --show-leak-kinds=all \
			--errors-for-leak-kinds=all --suppressions="$SUPP" "$BIN" "$@" 2>&1)
		if printf '%s' "$out" | grep -qE 'definitely lost: [1-9]|Invalid (read|write)|ERROR SUMMARY: [1-9]'
		then ko "$label [valgrind]" "$(printf '%s' "$out" | grep -E 'lost|Invalid|SUMMARY' | head -2 | tr '\n' ' ')"
		else ok "$label [valgrind clean]"; fi
	fi
}

# A good input must start without printing Error. The window stays open,
# so the run is killed by `timeout`: exit 124 counts as success. Exit 0 is
# also accepted (headless stub builds). No hidden --check-only flag: an
# undocumented argument is an easy way to lose the argc != 2 behaviour and
# evaluators do grep for such flags.
expect_ok() {
	local label=$1; shift
	local out code
	out=$(timeout "$TMO" "$BIN" "$@" 2>&1); code=$?
	if [ "$code" -eq 124 ] || [ "$code" -eq 0 ]; then ok "$label"
	elif [ "$code" -ge 128 ]; then ko "$label" "crashed (signal $((code - 128)))"
	else ko "$label" "exit $code: $(printf '%s\n' "$out" | head -2 | tr '\n' ' ')"; fi
}

norm_check() {
	echo "== norminette =="
	if command -v norminette >/dev/null; then
		norminette src inc libft | grep -v 'OK!' ; echo "(norminette exit: ${PIPESTATUS[0]})"
	else
		echo "norminette not installed (pipx install norminette)"
	fi
	echo "== heuristics (lines > 80 cols, > 5 functions/file, > 25 lines/function, > 5 locals, decl+assign) =="
	awk 'length($0) > 80 { printf "  %s:%d: %d cols\n", FILENAME, FNR, length($0) }' src/*.c inc/*.h libft/*.c libft/*.h
	for f in src/*.c libft/*.c; do
		n=$(grep -cE '^[a-z].*\)$' "$f"); [ "$n" -gt 5 ] && echo "  $f: $n functions"
		awk -v F="$f" '/^\{/ { s = NR; v = 0 } /^\t[a-z_]+\t+[*a-z_]+;$|^\t[a-z_ ]+\t+[*a-z_]+;$/ { if (NR > s) v++ }
			/^\}/ { if (NR - s - 1 > 25) printf "  %s: function ending line %d has %d lines\n", F, NR, NR - s - 1;
			        if (v > 5) printf "  %s: function ending line %d declares %d locals\n", F, NR, v }' "$f"
		grep -nE '^\t+[a-z_]+\t+\*?[a-z_]+ = ' "$f" | grep -v 'static' | sed "s#^#  $f: decl+assign? #"
	done
	echo "(empty list above = clean)"
}

if [ "$1" = "norm" ]; then norm_check; exit 0; fi
[ -x "$BIN" ] || { echo "binary $BIN not found: run make first"; exit 1; }

echo "== argument / file level =="
expect_error "no argument"
expect_error "two arguments"      maps/valid/basic.cub maps/valid/basic.cub
expect_error "directory as path"  maps
expect_error "directory named .cub" "$tmp/dir.cub" ; mkdir -p "$tmp/dir.cub"; expect_error "directory named .cub (exists)" "$tmp/dir.cub"
expect_error "nonexistent file"   maps/none.cub
expect_error "wrong extension"    maps/invalid/wrong_ext.cubx
expect_error "unreadable file"    "$tmp/noperm.cub"; cp maps/valid/basic.cub "$tmp/noperm.cub"; chmod 000 "$tmp/noperm.cub"; expect_error "unreadable file (chmod 000)" "$tmp/noperm.cub"; chmod 644 "$tmp/noperm.cub"

echo "== maps/invalid/*.cub =="
for f in maps/invalid/*.cub; do expect_error "$(basename "$f" .cub)" "$f"; done

echo "== generated stress inputs =="
{ printf 'NO ./textures/north.xpm\nSO ./textures/south.xpm\nWE ./textures/west.xpm\nEA ./textures/east.xpm\nF 1,2,3\nC 4,5,6\n\n'
  printf '1111111111\n'; i=0; while [ $i -lt 9998 ]; do printf '1000000001\n'; i=$((i + 1)); done; printf '1000N00001\n1111111111\n'; } > "$tmp/huge.cub"
expect_ok "10000-line map (valid)" "$tmp/huge.cub"
head -c 300000 /dev/urandom > "$tmp/binary.cub"; expect_error "binary garbage .cub" "$tmp/binary.cub"

echo "== maps/valid/*.cub (window opens, killed after ${TMO}s) =="
for f in maps/valid/*.cub; do expect_ok "$(basename "$f" .cub)" "$f"; done

echo
echo "passed: $pass  failed: $fail"
[ "$fail" -eq 0 ]
