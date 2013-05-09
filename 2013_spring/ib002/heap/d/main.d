
import std.stdio;

import Heap;

void main()
{
	int i;

	for (i=0; i<5; i++)
		writeln("hello '",i , "' world");

	Heap h;

//	writeln("new heap");
	h = new Heap();
//	writeln("created");

	h.add(5);
	h.add(3);
	h.add(7);
	h.dump();
	destroy(h);

	int []test_arr = [1, 2, 3, 4, 5, 6, 7];
	writeln("test ", test_arr[1..$], "arr");
//	writeln("destroyed");
	
}
