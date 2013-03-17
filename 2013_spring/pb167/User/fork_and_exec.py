#!/usr/bin/env python
from os import *

class Testovaci:
	def parent(self, child_pid):
		print 'parent: Child pid: {:d}'.format(child_pid)

		# Do some mess
		for x in range(0, 10):
			print x

		# Wait for child
		print "parent: waiting for child '{:d}'".format(child_pid)
		status = wait()
		print 'parent: child status: {:d}'.format(status[1])

	def child(self):
		print 'child'
		for x in range(20, 30):
			print x
		print 'child: execve "/bin/ls"'
		execv('/bin/ls', ['/some/very/long/path/to/ls'])
		print 'child: error during execve()'
		exit(1)

	def fork_and_something(self):
		pid = fork()
		if pid < 0:
			raise
		if (pid > 0):
			self.parent(pid)
		if (pid == 0):
			self.child()

t = Testovaci()
t.fork_and_something()

