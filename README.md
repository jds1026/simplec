g# simplec
A small and simple compiler front end for a C grammar subset. 
simplec is a one-pass compiler that implements recursive descent 
parsing. The compiler reads as input a .c file and builds an IR 
of it in the form of an abstract syntax tree.

---

## Installation
> git clone ht<span>tps://github.com/jds1026/simplec.git <br>
> cd simplec <br>
> make
  
---

## Usage
> ./simplec *somefile.c*

The compiler will create, open, and write to a file called
*compilation.txt* in the simplec directory. The output of the
compilation is an AST of each statement from the input source 
file, as well as the contents of the symbol table used during
these front end phases. 

For a statement like:
> int alpha = 4;

the generated output in *compilation.txt* would read: <br><br>
<img src="https://imgur.com/QickMr4.jpg" width="400"> <br>


The output displays the root node *=* first, followed by its 
two children nodes, *alpha* and *4*.<br>
Drawn as an AST, the output would read:<br><br>
<img src="https://imgur.com/sriYuX8.jpg" width="250"> <br>

Similarly, an expression such as: <br>
> int sum = alpha + 4 * 9;

would be generated in *compilation.txt* like so: <br><br>
<img src="https://imgur.com/Y4rvj4s.jpg" width="400"> <br>


and the AST would be drawn as: <br><br>
<img src="https://imgur.com/YgDhsOH.jpg" width="250">

