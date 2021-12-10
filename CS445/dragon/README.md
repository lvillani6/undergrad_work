# Dragon Project
### Lorenzo Villani
--------------------------------------------

### Design Document

| File | Description |
| --- | --- |
| Makefile | Includes make commands for compiling project. |
| dragon.l | Defines the Lexical Analyzer |
| dragon.y | Defines the Parser |
| semantic.c | Semantic Analyzer for dragon compiler. Checks pascal code against semantic rules. |
| hash.c | Implements a class of hash tables. |
| hash.h | Header file for hash.c |
| list.c | Implements a class of linked lists. |
| list.h | Header file for list.c |
| tree.c | Implements a class of binary trees. |
| tree.h | Header file for tree.c |

#### Design Details
**dragon.l** defines the rules for the Lexical Analyzer for converting the input file into a stream of tokens. These tokens are described in the Pascal Grammar document and in addition describe additional tokens for use in the parser.

**dragon.y** defines the grammar rules for the Parser as described in the Pascal Grammar document.

### User Manual
To use my program simply redirect the file to be analyzed to the program in terminal.  
`./dragon < file.p`

The output will be printed to stderr.

### Testing Report
I tested my code on all of the semantic test files provided. It successfully passes or throws the relevant errors for the first four classes of semantic rules:
1. Scoping
2. Expressions
3. Statements
4. Arrays


### Status Report
As it stands right now my program is only a syntax analyzer and does not have and code generation. The next step of course would be to implement code generation.

My program does not successfully handle the correct semantic analysis for function and procedure calls. It does ensure that functions return the right type, at least on single statement bodies. However it does not have functionality to check the parameters of the function or the procedure.

Also I would like to store the main function as the scope owner for the main scope just for completeness, which my program currently does not do.

### Haiku
> To generate code  
> with Bison and Flex  
> and slay a dragon.  
