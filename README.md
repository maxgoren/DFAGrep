# DFAGrep
A fast DFA based regular expression matcher


## Supported Operators
DFAGrep supports the full suite of extended regular expressions, 
with each operator functioning as follows:

| operator | purpose|
|----------|--------|
|  rq     | match expression r followed by expression q |
| r1\|r2   | match expression r1 or expression r2 |
| \[...\] | match any character which appears in the brackets, can include ranges like A-Z or 0-9 |
| \[^...\] | negated character class, matches any character _not_ in the brackets, may also include ranges |   
|  r*     | match expression 'r' 0 or more times |
|  r+     | match expression 'r' 1 or more times|
|  r?    | match expression 'r' 0 or 1 time |
| ^r     | match must begin the line|
|  r$     | match must end the line|
| (r)     | expressions can be grouped with parentheses, can be used to override operator precedence|


## Character Class Shortcuts
DFAGrep has support for perl-style shortcuts on character classes where escaped
lower case letters represent a character class, and their uppercase equivelant represents
their negated class.


| Character | Purpose |
|-----------|---------|
|     d     | match digits 0-9 |
|     D     | match all _except_ 0-9 |
|     w     | match any from 0-9, A-Z, a-z, '_' |
|     W     | match any _except_ those listed above |
|     s     | match white space characters (' ', '\t', '\r', '\n') |
|     S     | match any non-white space character |
