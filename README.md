# DFAGrep
A fast DFA based regular expression matcher

| operator | purpose|
|----------|--------|
|  rq     | match expression r followed by expression q |
| r1\|r2   | match expression r1 or expression r2 |
| \[...\] | match any character which appears in the brackets, can include ranges like A-Z or 0-9 |   
|  r*     | match expression 'r' 0 or more times |
|  r+     | match expression 'r' 1 or more times|
|  r?    | match expression 'r' 0 or 1 time |
| ^r     | match must begin the line|
|  r$     | match must end the line|
| (r)     | parentheses can be used to override operator precedence|
