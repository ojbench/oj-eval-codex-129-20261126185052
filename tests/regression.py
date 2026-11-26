"""Run focused interpreter regressions against the compiled executable."""
import pathlib
import subprocess
import sys


def check(commands, expected):
    data = str(len(commands)) + "\n" + "\n".join(commands) + "\n"
    result = subprocess.run(
        [str(pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else './code').resolve())],
        input=data, text=True, capture_output=True, check=True,
    )
    assert result.stdout == '\n'.join(expected) + '\n', result.stdout


check([
    'Dedent', 'Declare int a -2147483648', 'Print a',
    'Declare int a 9', 'Declare int bad 12x', 'Print bad',
    'Declare int 9name 1', 'Declare float f 2',
    'Indent', 'Declare string a "  a b  "', 'Print a',
    'SelfAdd a 3', 'SelfAdd a "!"', 'Add a a a', 'Print a',
    'Declare int b +2147483647', 'Add b b a', 'Print b',
    'Dedent', 'Print a', 'Print b', 'SelfAdd a +1', 'Print a',
    'Declare int bad 7', 'Print bad',
    'Declare string empty ""', 'SelfAdd empty " "', 'Print empty',
], [
    'Invalid operation', 'a:-2147483648',
    'Invalid operation', 'Invalid operation', 'Invalid operation',
    'Invalid operation', 'Invalid operation', 'a:  a b  ',
    'Invalid operation', 'a:  a b  !  a b  !',
    'Invalid operation', 'b:2147483647', 'a:-2147483648',
    'Invalid operation', 'a:-2147483647', 'bad:7', 'empty: ',
])
print('Regression tests passed')
