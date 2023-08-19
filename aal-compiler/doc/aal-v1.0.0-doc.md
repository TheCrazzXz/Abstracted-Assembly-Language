# ABSTRACTED ASSEMBLT LANGUAGE (x86 32-bits)

Abstracted Assembly Language (V1.0.0) doc

## AAL
Abstracted Assembly Language is a compiled and functionnal programming language.  
Please note that currently there's only one unique architecture that the lanugage supports : x86 32-bit for linux. Which you can use on a 64-bit machine or a 32-bit machine, but not an ARM machine.

## How to use AAL
To compile some aal code, we first have to code in an AAL file, where the code for the program will be stored, what is refered as the **source code** for our program.  
Once the code is written in it, we can compile it and then have our program.  
To compile an AAL program, the AALC (aal compiler) tool is used, it's stored in "../bin/aalc" when you have compiled the compiler.  
If you haven't built the compiler yet, use "../compile.sh" and the compiler is built and available in "../bin/aalc"
To compile an AAL program : `../bin/aalc <PROGRAM SOURCE FILE.aal> -o <OUTPUT EXECUTABLE>`

## INTRO
In AAL, there are what we call functions. Functions are peice of code that are executed when they are called by other functions.  
To execute some code in aal in the first place, one has to define the `entry` function, this a function is a special function because it's the first function that's automaticly called at the start of the program, so all the starting code of the program, all the main code of the program is there.
To define it we first have to declare it :
```aal
func entry();
```
This tells the compiler that we will define the entry function.  
Then once it's declared, we can define it, putting its code in it :
```aal
func entry:
{
	(ALL THE CODE OF THE ENTRY FUNCTION COMES THERE);
}
```

## COMMENTS
Comments are piece of the code that the compiler ignores.  
There are two types of comments, one line comment and multiple-line comments.  
To comment a whole line (one line comment) use `//` at the very start of this line : 
```aal
// this line is a comment and thus just ignored by the compiler
```
To comment multiple lines use `///` to begin and `///` again to end commented region :
```aal
///
	Hey ! What's up ???


	This code is ignored

	Yet again

	Still ignored
///
```

## Variables
Variables are labels that hold data, so we can use them to store data, for example if we're making a program that asks the name of the user or its age, we have to store them in memory so we can use them later.  
Each function has its own variables and we have to tell AAL what are the variables we're gonna use at the very start of each function. Variables of other functions aren't accessible in a function because each a function ends, its variables are destroyed, which we call a stack frame.  
We can create variables of different **data types** :
- `byte` : Store one byte in the memory, holding 8 bits, so numbers that go from 0 to 255. This type can be used for example to store an ascii character.
- `dword` or `pointer` : Store a 32-bit number, holding 4 bytes, which means a numbers that goes from 0 to 4 294 967 295

Each variable has to be declared at the very start of each function, including the entry function.  
Variables are declared this way :
```
<type> <variable name>;
```
The type has to be one the data types aforementionned and the variable name has to follow these rules :
- Can be alphabet (from a to z or from A to Z) : example : testvariable
- Can have underscores (`_`) : example : user_name
- Can't have special characters or numbers or any other thing than alphabet and underscores.  
These naming rules are what we'll call **symbol naming rule**.  

Example :
```aal
dword user_age;
dword some_number;
dword another_number;
byte a_character;
byte another_character;
```
The declared variables declared all together at the very start of a function can be used later in the code.  

After declarations :  

To set/assign a variable to a number :
```aal
<variable name> = <number>
```
Example : 
```aal
user_age = 255;
some_number = 985;
another_number = 245;
a_character = 97
```
`byte` variables can be set to ascii characters :
```aal
<variablme name> = '<one ascii character>';
```
Example :
```aal
a_character = 'z';
```

We can set a variable to an expression :
```aal
<variable name> = <expression>
```

It's possible to set the value of an existant variable anywhere in the function code after declarations

## EXPRESSIONS
Expressions are a sequence of variables, numbers, operators or strings.  
In expressions we can have operators that we can use between variables...

## OPERATORS
Operators are special symbols that tells the compiler what to do between variables :
**Arithmetic operators** :
- `+` : Adds the previous result and next variable together
- `-` : Subtracts the previous result and next variable together
- `*` : Multiplies the previous result and next variable together
- `/` : Divides the previous result by next variable
**Bitwise operators** :
- `|` : Performs a bitwise OR between the previous result and next variable together
- `^` : Performs a bitwise XOR between the previous result and next variable together
- `~` : Performs a bitwise AND between the previous result and next variable together
**Comparaison operators** :
- `==` : Returns one if the previous result and next variable are equal in number
- `>` : Returns one if the previous result is superior than next variable are equal in number
- `<` : Returns one if the previous result is inferior than next variable are equal in number
- `>=` : Returns one if the previous result is superior or equal than next variable are equal in number
- `<=` : Returns one if the previous result is inferior or equal than next variable are equal in number


Please note that the way this language uses operators is not mathematical, as every operator acts between everything that was specified and before and next so divisions are multiplications aren't computed first :
In aal :
```aal
[12 + 2 / 2] = 14 / 2 = 7
```
Would, evaluated as in maths :
```
12 + 2 / 2 = 12 + 1 = 13
```
If we actually want to process as in maths we would do it this way :
```
[2 / 2 + 12] = 1 + 12 = 13
```

## EXPRESSIONS (back)
To use expressions, we have to use square brackets (`[`, `]`), an expression starts and ends with square brackets
Pattern is the following : 
```aal
[<variable or number> <operator> <variable or number> <operator> ...]
```
Example :
```aal
[user_age + 255 / 2 + 6 * 2 + some_number]
```
It's of course possible not to have any operator :
```aal
[<variable or number>]
```

## VARIABLES (back)
So then to set a variable to another variable you would do :
```aal
<destination variable name> = [<source variable name>];
```
To increment (add one) to a variable :
```aal
<variable name>+;
```

To decrement (subtract one) to a variable :
```aal
<variable name>-;
```

## CONDITIONNAL STATEMENTS :
It's possible to execute some piece of code inside a function only if a certain expression is tested equal to 1, it's done via the `test` keyword :
```aal
test <EXPRESSION>
{
	(CODE THAT WILL BE EXECUTED ONLY IF EXPRESSION IS TESTED EQUAL TO 1 (true))
}
```
It's also possible, inside a conditionnal statement, to define the bahaviour of the code if the expression is tested equal to 0 using the `fail` statement :
```aal
test <EXPRESSION>
{
	(CODE THAT WILL BE EXECUTED ONLY IF EXPRESSION IS TESTED EQUAL TO 1 (true))
}
fail
{
	(CODE THAT WILL BE EXECUTED ONLY IF EXPRESSION IS TESTED EQUAL TO 0 (false))
}
```
Example :
```aal
func entry(dword argc pointer prog_name pointer number_str);
func entry:
{
	dword n;
	dword m;
	dword result;

	m = 4;
	n = 2+2+m;

	test [n >= 6]
	{
		result = 1;
	}
	fail
	{
		result = 0;
	}
}
```
The `n` variable is set to `2 + 2 + m`, thus `2 + 2 + 4` which is `8`, and 8 is superior than 6 so the expression will be `1` at the end so result would be set to 1, if m was for example `1`, the fail statement code would be executed instead and result would be set to `0`.  
It's possible to have other conditionnal statements inside other conditionnal statements.  

## LOOPS
Loops are like conditionnal statements but with the difference that a certain piece of code is executed while a condition is true instead of just one time.  
Loops are acheived using the keyword `do` and `loop`
The `break` keyword can be used inside a loop to unconditionnaly stop it.
```aal
do
{
	(code that will be executed while the expression is tested equal to 1)
}
loop <expression>
```
Each execution of the loop code is called an iteration of the loop
Example :
```aal
dword n;
dword some_number;
dword other;

some_number = 5;

do
{
	other = 1;
}
loop [some_number == 6]
```
The piece of code inside the loop will be executed while the `some_number` variable is equal to 6, which of course in this case always false so the loop will not even be executed

```aal
dword n;
dword some_number;
dword other;

some_number = 6;

do
{
	other = 1;
}
loop [some_number == 6]
```
Here the loop is always executed, because it will never end  

Basically the program will do this :
- Iteration 1 : Is some_number equal to 6 ? Yes, so we execute what's inside the loop
- (set other to 1)
- Iteration 2 : Is some_number equal to 6 ? Yes, so we execute what's inside the loop
- (set other to 1)
- Iteration 3 : Is some_number equal to 6 ? Yes, so we execute what's inside the loop
- (set other to 1)
- Iteration 4 : Is some_number equal to 6 ? Yes, so we execute what's inside the loop
- (set other to 1)
- Iteration 5 : Is some_number equal to 6 ? Yes, so we execute what's inside the loop
- (set other to 1)
...
It's not gonna end until we break the program.
Here's another example :
```aal
dword i;
dword other;

i = 0;

do
{
	other = [other + i];
	i+;
}
loop [i < 20]
```
This is an example of what we call a counter loop, it will execute the loop code n times by setting some counter (here `i`) to 0 before the loop, at each iteration incrementing (add one to) the counter and checking if the counter is less than n.  
So it will execute the loop code 20 times and at each time `i` will be set to `<iteration number>-1`, so at first iteration `i` will be 0, at second it will be 1, at third it will be 2... until 20th iteration were i will be 19 at the start and 20 at the end (after incrementing).

## ARRAYS
Arrays are fixed-sized list of variables with same type.  
They are list of n variables that we can use in our code, we can select one of these variables in the array.  

To declare an array :
```aalc
(type for variables) (array name)<(number of elements)>
```
Parentesis are used not to confuse with `>` and `<`.  
Example :
```aalc
dword five_numbers<5>;
byte two_hundred_chars<200>;
```
The `five_numbers` array holds 5 dword numbers and the `two_hundred_chars` array holds 200 bytes.  

Each of the variables in an array are associated with an index, the first variable would have the index 0, the second the index 1, the third the index 2... Each variable of the array would have the index `<place>-1`  
To access a certain index of the array :
```aalc
(array)<(index)>
```
Parentesis are used not to confuse with `>` and `<`.  

Example :
```aalc
// set first
five_numbers<0> = 500;

// set fifth
five_numbers<4> = 980;

// set first
five_numbers<0> = [five_numbers<4> + 1];

// set 151th
two_hundred_chars<150> = 'a';

// set 11th
two_hundred_chars<10> = 'z';
```
Specifying only the name of the array as `<array>` would specify the first variable inside it.  

## POINTERS
Pointers are dword numbers that store the address of other variables.  
- Storing the address of a variable means to point on the variable.  
- Dereferencing a pointer means to access the variable we're pointing at.  

Pointers can point to any variable but when we want to dereference them we have to tell the compiler what is the type of the derefenced variable.  
Because pointing to a variable just means to store the address of a variable, there's a special operator in AAL that gets the address of a variable (`&`) :
```
&<variable name>
```
By doing this we specify the address of the variable.  

To declare a pointer :
```aal
pointer <pointer name>;
```
A pointer is a `dword` exactly a dword variable, it holds a 32-bit address, then a 32-bit number.  
Now to make a point point on a certain variable it's done the following way :
```aal
<pointer name> = [&<variable name to point to>];
```
Dereference type :
- `b` : dereference a byte variable
- `d` : dereference a dword variable
To dereference a pointer we have to specify the type of the dereference and then the `$` and then the pointer name :  

```aal
<dereference type>$<pointer name>
```
Now we can use this as a variable in expressions, or setting value to it :
```aal
<dereference type>$<pointer name> = <expression>;
```
Or :
```aal
... = [<<dereference type>$<pointer name>]
```
Example :
```aal
func entry(dword argc pointer prog_name pointer number_str);
func entry:
{
	pointer ptr;
	dword a;
	dword b;
	dword result;

	a = 0;
	b = 0;

	// a = 0 and b = 0
	result = [a+b];

	// point ptr to the a
	ptr = [&a];

	// dereference the pointer as dword then get the value of the pointed dword variable, then a
	result = [d$ptr];

	// now result is equal to a, then 0

	// point ptr to the b
	ptr = [&b];

	// dereference the pointer as dword then get the value of the pointed dword variable, then b and set it to value
	d$ptr = 589;

	// now b is set to 589

	// a = 0 and b = 589, we've indirectly changed the value of b
	result = [a+b];

	// now result is equal to 589
}

```

## FUNCTIONS
We've already talked about the entry function, which is executed first in our program, let's now talk about how to create other functions.
To declare a function (outside the entry function) :
```aal
func <function name>(<parameter list*>);
```
The function name has to follow the **symbol naming rule**.  

Parameter list is the list of function parameters, function parameters are variables that are set when the function is called and that can be accessed inside the function itself, to define parameters it's done the following way :
Just specify the type of a parameter followed by it's name
```
<parameter 1 type> <parameter 1 name> <parameter 2 type> <parameter 2 name> ...
```
To tell the compiler not to use any parameter just don't type any parameter specifier :
```
```
Then to define function code :
```aal
func <function name>:
{
	(function code...)
}
```
Inside a function we can declare variables, every variable has to be declared at the very start of the function :
```
func ...:
{
	<type> <var1>;
	<type> <var2>;
	<type> <var3>;
	...
}
```
We can also not declare any variable.  

Inside this function code we can use as variables the parameters if there are parameters.  
To call a function :

```aal
<function name>(<argument list*>)
```
Argument list is the followed-by-each-other value corresponding to each parameter, so just like variables values, they are expressions :
```
<expression : argument 1> <expression : argument 2> ...
```
Each nth argument correspond to the nth parameter of the function.  
Notice : Functions can indeed call themselves.  

Function return is done using the `return` keyword, this instruction will output a dword from the function and leave the function, to output in this case means that if some variable was set to the call of this function, then it will be set to the value that it's returning.
To return without outputing a value (only leaving the function) :
```aal
return;
```
To return a value (leaving the function and output the value) :
```
return <expression>;
```
Expression has to be a square brackets expression.  

Example :
```aal
func add_two_numbers_and_multiply_the_result_by_two(dword a dword b);
func add_two_numbers_and_multiply_the_result_by_two:
{
	return [a + b / 2];
}

func entry(dword argc pointer prog_name pointer number_str);
func entry:
{
	dword result;
	dword n;

	n = 7;

	result = [add_two_numbers_and_multiply_the_result_by_two(1 [2+n])];
}
```
The function should then return 5 and thus result would be set to 5;