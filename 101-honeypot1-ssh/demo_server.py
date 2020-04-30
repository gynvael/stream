#!/usr/bin/env python

# Copyright (C) 2003-2007  Robey Pointer <robeypointer@gmail.com>
# With additions by Gynvael Coldwind for educational purposes of honeypotting ;)
#
# This file is part of paramiko.
#
# Paramiko is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# Paramiko is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with Paramiko; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.

import gc
import base64
from binascii import hexlify
import os
import socket
import sys
import threading
import traceback
import functools

import paramiko
from paramiko.py3compat import b, u, decodebytes


# setup logging
paramiko.util.log_to_file("demo_server.log")

host_key = paramiko.RSAKey(filename="test_rsa.key")
# host_key = paramiko.DSSKey(filename='test_dss.key')

#print("Read key: " + u(hexlify(host_key.get_fingerprint())))

def disable_stdout_buffering():
    # Appending to gc.garbage is a way to stop an object from being
    # destroyed.  If the old sys.stdout is ever collected, it will
    # close() stdout, which is not good.
    gc.garbage.append(sys.stdout)
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)

class Server(paramiko.ServerInterface):
    def __init__(self, plog):
        self.event = threading.Event()
        self.plog = plog

    def check_channel_request(self, kind, chanid):
        self.plog("check_channel_request: kind='%s'" % kind)
        if kind == "session":
            return paramiko.OPEN_SUCCEEDED
        return paramiko.OPEN_FAILED_ADMINISTRATIVELY_PROHIBITED

    def check_auth_password(self, username, password):
        self.plog("check_auth_password: username='%s', password='%s'" % (
            username, password
        ))
        if username and password:
            return paramiko.AUTH_SUCCESSFUL
        return paramiko.AUTH_FAILED

    def check_auth_publickey(self, username, key):
        self.plog("check_auth_publickey: username='%s', key fingerprint='%s'" % (
            username, u(hexlify(key.get_fingerprint()))
        ))
        return paramiko.AUTH_FAILED

    def check_auth_gssapi_with_mic(
        self, username, gss_authenticated=paramiko.AUTH_FAILED, cc_file=None
    ):
        self.plog("check_auth_gssapi_with_mic: username='%s'" % (
            username
        ))
        return paramiko.AUTH_FAILED

    def check_auth_gssapi_keyex(
        self, username, gss_authenticated=paramiko.AUTH_FAILED, cc_file=None
    ):
        self.plog("check_auth_gssapi_keyex: username='%s'" % (
            username
        ))
        return paramiko.AUTH_FAILED

    def enable_auth_gssapi(self):
        return False

    def get_allowed_auths(self, username):
        return "password,publickey"

    def check_channel_shell_request(self, channel):
        self.plog("check_channel_shell_request:")
        self.event.set()
        return True

    def check_channel_subsystem_request(self, channel, name):
        self.plog("check_channel_subsystem_request: name='%'" % name)
        return False

    def check_channel_forward_agent_request(self, channel):
        self.plog("check_channel_forward_agent_request:")
        return False

    def check_channel_pty_request(
        self, channel, term, width, height, pixelwidth, pixelheight, modes
    ):
        self.plog("check_channel_pty_request: term='%s', %ix%i (pixels: %ix%i)" % (
            term, width, height, pixelwidth, pixelheight
        ))
        return True


def printlog(client_id, s):
    with printlog.lock:
      print('%s %s' % (client_id, s))
printlog.lock = threading.Lock()

def main():
    disable_stdout_buffering()

    # now connect
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("", 2200))
    except Exception as e:
        print("*** Bind failed: " + str(e))
        traceback.print_exc()
        sys.exit(1)


    sock.listen(100)
    print("Listening for connection ...")

    while True:

        try:
            client, addr = sock.accept()
            client_id = `addr`
        except Exception as e:
            print("*** Listen/accept failed: " + str(e))
            traceback.print_exc()
            sys.exit(1)

        plog = functools.partial(printlog, client_id)

        plog("Got a connection!")

        t = paramiko.Transport(client)
        try:
            t.load_server_moduli()
        except:
            plog("(Failed to load moduli -- gex will be unsupported.)")
            raise
        t.add_server_key(host_key)
        server = Server(plog)
        try:
            t.start_server(server=server)
        except paramiko.SSHException:
            plog("*** SSH negotiation failed.")
            sys.exit(1)

        th = threading.Thread(
            target=handler_ssh_connection,
            args=(t, server, plog)
        )
        th.daemon = True
        th.start()

def handler_ssh_connection(t, server, plog):
    try:
        handler_ssh_connection_worker(t, server, plog)
    except socket.error:
        pass

def handler_ssh_connection_worker(t, server, plog):
    plog("handler_ssh_connection()")

    # wait for auth
    chan = t.accept(20)
    if chan is None:
        plog("*** No channel.")
        sys.exit(1)
    plog("Authenticated!")

    server.event.wait(10)
    if not server.event.is_set():
        plog("*** Client never asked for a shell.")
        sys.exit(1)

    chan.send(
"""Welcome to Ubuntu 18.04.4 LTS (GNU/Linux 4.15.0-96-generic x86_64)

 * Documentation:  https://help.ubuntu.com
 * Management:     https://landscape.canonical.com
 * Support:        https://ubuntu.com/advantage

  System load:  0.0                Processes:              91
  Usage of /:   30.9% of 19.56GB   Users logged in:        2
  Memory usage: 3%                 IP address for enp1s4:  192.168.13.124
  Swap usage:   0%                 IP address for docker0: 172.17.0.1


22 packages can be updated.
0 updates are security updates.


Last login: Thu Apr 16 18:30:48 2020
""".replace("\r\n", "\n").replace("\n", "\r\n"))

    f = chan.makefile("rU")

    PROMPT = "production:~$ "
    chan.send(PROMPT)

    line = ""

    while True:
        ch = f.read(1)
        line += ch
        if ch == '\r':  # num-enter?
            plog("SHELL_LINE: %s" % `line`)
            line = ""
            chan.send("\r\n")
            chan.send(PROMPT)
        else:
            chan.send(ch)

    chan.close()


if __name__ == "__main__":
    main()

