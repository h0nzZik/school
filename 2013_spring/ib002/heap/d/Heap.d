import std.stdio;

class Heap {
	int []nodes;

	this() {
	//	nodes.length = 0;
	//	writeln("heap constructor");
	}

	~this()
	{
	//	writeln("heap destructor");
	}
	void bottom_up(ulong id)
	{
		ulong parent;

		writeln("bottom up ", id);
		parent = (id - 1) / 2;
		while (id != 0 && nodes[id] > nodes[parent]) {
			int tmp;
			writeln("switching '", id, "' and '", parent, "'");
			tmp = nodes[parent];
			nodes[parent] = nodes[id];
			nodes[id] = tmp;

			id = parent;
			parent = (id - 1) / 2;
		}
	}


	void add(int x)
	{
		nodes.length++;
		nodes[$-1] = x;
		bottom_up(nodes.length - 1);
	}


	void dump()
	{
		foreach(ulong i, int x; nodes) {
			if (i)
				writeln("([",(i-1)/2,"] = ",nodes[(i-1)/2],") -> ([", i,"] = ", x ,")");
			else
				writeln("[0] = ", x, "");
		}
	}
}
