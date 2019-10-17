# Variable
```
// declaration of Hello variable
Hello := "hello, world!";
// Hello variable is reused
Hello = "Wow!";
```

# Object
```
myObj := {
    a: 1,
    b: "yay"
};

objWithFn := {
    sub: n to: m {
        m - n
    }
};

myObj.a             <-> myObj["a"]
myObjWithFn.#sub:to <-> myObjWithFn["sub:to"]
```

# Array
```
myArray := [];
myArray[10] = True;
myArray[0] = "hello";
myArray[1] = [1, 2, 3];
```

# Blocks
__An empty block with no arguments would have no meaning so `{}` is treated as an empty object.__
```
myBlock := {
    <statement 1>;
    ...
    <statement n-1>;
    5 + 2 // no ";" at the end of the line means "return <expression>"
};

blockWithArgs := { | a b c |
    ...
};
```

## Function
A function is a named block that is invoked with a particular message to its container.
Can only be declared in an object litteral.
```
obj := {
    add: n to: m { // the message is add:to, the invoked block takes 2 arguments
        n + m
    }
};
```
## Returning from blocks into a function
```
myFunction {
    ...
    {i == 2}
        ifTrue: {
            ...
            Null // returns to the parent function
        }
        ifFalse: {
            ...
            ^123 // returns from the parent function
        }
    
    321 // only reached if i == 2
};
```

# Class
```
// declaration
```
MyClass := MyParent subclass: {
    b: 1337,
    function {
        a + b
    }
};

MyClass := MyParent subclassWithStatic: { 
    a: 1,
    myStaticFunction {
        a * 2
    }
    // "new" message should be put here
} withInstance: {
    b: 1337,
    function {
        a + b
    }
};
```

// instanciation
myObj := MyClass new;
```

# Special objects
```
this
super <-> this.__super__
mod
__scope__ <-> __context__.__scope__
__context__
```

# Syntatic sugars
## Variable
```
Hello := "hello, world!";

// syntatic sugar for
__scope__.Hello = "hello, world!";
```

## Array
```
myArray := [1, 2, 3];

// syntatic sugar for
myArray := Array new;
myArray push: 1;
myArray push: 2;
myArray push: 3;
```

# Scratchpad
```
mod import: "pipou.date.Date" as: "Date";
mod import: "myProgram.hello.World";

function := {
    {
        ...
    } on: Exception do: { | e |
        ...
    } finally: {
        ...
    };
};
mod export: function as: "function";
```
