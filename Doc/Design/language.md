# Variable
```
Hello = "hello, world!";
```

# Object
```
myObj = {
    a: 1,
    b: "yay"
};

objWithFn = {
    sub: n to: m {
        m - n
    }
};

myObj.a             <-> myObj["a"]
myObjWithFn.#sub:to <-> myObjWithFn["sub:to"]
```

# Array
```
myArray = [];
myArray[10] = True;
myArray[0] = "hello";
myArray[1] = [1, 2, 3];
```

# Blocks
__An empty block with no arguments would have no meaning so `{}` is treated as an empty object.__
```
myBlock = {
    <statement 1>;
    ...
    <statement n-1>;
    5 + 2 // no ";" at the end of the line means "return <expression>"
};

blockWithArgs = { a b c |
    ...
};
```

## Function
A function is a named block that is invoked with a particular message to its container.
```
add: n to: m { // the message is add:to, the invoked block takes 2 arguments
    n + m
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
MyParent subclass: "MyClass"
    with: {
        b: 1337,
        function {
            a + b
        }
    }

MyParent subclass: "MyClass"
    withStatic: { 
        a: 1,
        myStaticFunction {
            a * 2
        }
        // "new" message should be put here
    }
    withInstance: {
        b: 1337,
        function {
            a + b
        }
    }

// Anonymous class
theClass = MyParent subclass: ""
    with: {
        b: 1337,
        function {
            a + b
        }
    }
```

// instanciation
myObj  = MyClass new;
myObj2 = theClass new;
```

# Special objects
```
this  <-> __scope__.__super__
super <-> __scope__.__super__.__super__
__scope__
__context__
__module__
```

# Syntatic sugars
## Variable
```
Hello = "hello, world!";

// syntatic sugar for
__scope__.Hello = "hello, world!";
```

## Function
```
add: n to: m {
    n + m
};

// syntatic sugar for
__scope__.#add:to = { n m |
    __scope__.n + __scope__.m
};
```

# Scratchpad
```
import: "pipou.date.Date" as: "Date"; // with __scope__ == __module__
                                      // <-> __module__ import: ... as: ...;
import: "myProgram.hello.World";

function {
    {
        ...
    } catch: { e |
        ...
    } finally: {
        ...
    }
};
```
