#!/usr/bin/python

# helper script for sublime text, so that we can call make even in a subdirectory

import os
import subprocess

# recurses up until makefile is found
def find_makefile():
	current_dir = os.getcwd()
	home_dir = os.environ["HOME"]

	files = os.listdir('.')


	# go directory up till we either are in home or a makefile has been found
	while('makefile' not in files and current_dir != home_dir and current_dir != '/'):
		os.chdir('../')
		current_dir = os.getcwd() 
		files = os.listdir(current_dir)

def main():
	find_makefile()

	# call make when search has finished
	subprocess.call(['make'])

if __name__ == '__main__':
	main()
