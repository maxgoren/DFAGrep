# DFAGrep
A fast DFA based regular expression matcher

| operator | purpose|
|----------|--------|
|  rq     | match r followed by q |
| r1\|r2   | match r1 or r2 |
| \[...\] | match any character which appears in the brackets, can include ranges like A-Z or 0-9 |   
|  r*     | match r 0 or more times |
|  r+     | match r 1 or more times|
|  r?    | match r 0 or 1 time |
| ^r     | start match at beginning of line|
|  r$     | match at the end of line|
| (r)     | parentheses can be used to override operator precedence|
