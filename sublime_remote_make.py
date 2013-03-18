#!/usr/bin/python

import os
import subprocess
import argparse

import sublime_make

# sshes into host as user and executes args
def  make_remote(user, host, args):
    subprocess.call(['ssh', user + '@' + host, args])

# extracts args from commandline
def parse_args():
    parser = argparse.ArgumentParser(description="bam test runner")
    parser.add_argument('--host', nargs='+', help='enter the host you want to login to', default = ['localhost'], dest='host')
    parser.add_argument('--user', nargs='+', help='enter the user you want to use to login', default =[os.environ['USER']], dest='user')
    parser.add_argument('--args', nargs='+', help='enter the commands you want to execute', default = ['make'], dest='args')

    args = parser.parse_args()

    return "".join(args.user), "".join(args.host), "".join(args.args)

# returns args with added cd current dir on remote host
def change_to_working_dir(args):
    sublime_make.find_makefile()

    current_dir = os.getcwd()
    home_dir = os.environ['HOME']

    # no cd necessary if target is home dir
    if home_dir == current_dir:
        return args

    current_dir = current_dir.replace( home_dir + '/', '')
    return 'cd ' + current_dir + ' ; ' + args  

def main():
    user, host, args = parse_args()
    args = change_to_working_dir(args)
    make_remote(user, host, args)

if __name__ == '__main__':
    main()
