## Language
A simple programming language for my virtual processor.

#### Functions
Each program must contain a main function. To declare a function, type funtion name, function arguments in square brackets and function body in curly brackets.
Function name must start with a letter and only can contain numbers and letters.
Each argument must be followed by | symbol. If function has no arguments, leave square brackets empty. 
Each function must contain return command followed by math expression.
Functions are called by typing function name followed by parameters in square brackets separated by |. Only variables can be passed as parameters. If function has no arguments, leave the brackets empty. 
![Function declaration](screenshots/func_decl.png)

#### Variables
Variables are declared by var command followed by variable name. Variable name must start with a letter and only can contain numbers and letters. @ is an assignment operator, math expression on the left is assigned to variable on the right.
read and write commands are used to read and write numbers to variables.
![Variables](screenshots/variables.png)

#### Math operators
Math operators are addition(+), substraction(-), multiplication(*), division(/), sine(sin), cosine(cos), square root(sqrt) and absolute value(abs).

#### Conditional operator and while cycle
There are no logical operators. In ifs and whiles, math expression is used as condition. If math expression equals to 0 the condition is false, the condition is true otherwise.
