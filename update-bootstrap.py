#!/usr/bin/python
# Update bootstrap nodes
#
# This script is used to generate src/tox_bootstrap.h by adding a list
# of bootstrap nodes from https://nodes.tox.chat/
#
# It should be executed on a regular basis (before a release) to make sure
# the list is up to date and contains active bootstrap nodes.
# This will make sure clients can connect to the network quickly and do not have to waste
# time trying to connect to nodes that do no longer exist.
#
# You can run the script like this:
#
# python3 tools/update-bootstrap.py > src/tox_bootstrap.h
#
# Status information will be printed to stderr.
#

import http.client
import json
from datetime import datetime
import re
import sys

# print for stderr


def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

# check whether arg is an IP or a hostname


def is_ip(ip):
    # these are not exactly nice patterns but the are sufficient to distinguish IP from hostname
    ipv4 = re.compile("^\\d+\\.\\d+\\.\\d+\\.\\d+$")
    ipv6 = re.compile("^[0-9a-f:]+$")
    return ipv4.match(ip) or ipv6.match(ip)

# select a tcp port from a given range, use 443 if available


def select_tcp_port(ports):
    if ports.count(443) > 0:
        return 443
    else:
        return ports[0]

# http://stackoverflow.com/questions/18854620/whats-the-best-way-to-split-a-string-into-fixed-length-chunks-and-work-with-the#18854817


def chunkstring(string, length):
    return (string[0+i:length+i] for i in range(0, len(string), length))


# get latest node data
connection = http.client.HTTPSConnection('nodes.tox.chat')
connection.request('GET', '/json')
response = connection.getresponse()

nodeData = response.read().decode()
data = json.loads(nodeData)

# print some info
eprint("Last scan: " + datetime.fromtimestamp(data.get('last_scan')).isoformat(' '))
eprint("Last refresh: " +
       datetime.fromtimestamp(data.get('last_refresh')).isoformat(' '))
eprint("Nodes: " + len(data.get('nodes')).__str__())

# filter out offline nodes
# only keep nodes that are active on tcp and udp
# also filter nodes that specify a hostname instead of IP
# we do not want utox to make DNS queries
# some of those are on a DynDNS so it does not even make sense to do the query in this script
nodes = []
for n in data.get('nodes'):
    if not n.get('status_udp') or not n.get('status_tcp'):
        continue
    if is_ip(n.get('ipv4')):
        nodes.append({
            'ip': n.get('ipv4'),
            'ipv6': 'false',
            'udp': n.get('port'),
            'tcp': select_tcp_port(n.get('tcp_ports')),
            'version': n.get('version'),
            'pubkey': n.get('public_key'),
            'maintainer': n.get('maintainer'),
            'location': n.get('location'),
        })
    if is_ip(n.get('ipv6')):
        nodes.append({
            'ip': n.get('ipv6'),
            'ipv6': 'true',
            'udp': n.get('port'),
            'tcp': select_tcp_port(n.get('tcp_ports')),
            'version': n.get('version'),
            'pubkey': n.get('public_key'),
            'maintainer': n.get('maintainer'),
            'location': n.get('location'),
        })

eprint("filtered offline and hostname-only nodes: ",
       len(nodes), "candidate entries")

# sort by the following criteria:
# - 1. sort by version, prefer nodes that are up to date
# - 2. prefer low udp port, i.e. 443 gets listed higher
# - 3. prefer tcp with port 443 over tcp without it
eprint("sorting by criteria...")
nodes = sorted(nodes, key=lambda n: (n.get('version'), -
               n.get('udp'), 1 if n.get('tcp') == 443 else 0), reverse=True)

f = sys.stdout
f.write("""#ifndef TOX_BOOTSTRAP_H
#define TOX_BOOTSTRAP_H
//
// IMPORTANT: This file is generated by the /tools/update-bootstrap.py script, do not edit manually.
//
struct bootstrap_node {
    const char *address;
    bool ipv6;
    uint16_t port_udp;
    uint16_t port_tcp;
    uint8_t key[32];
} bootstrap_nodes[] = {
""")

# use the first 32 nodes that match the criteria above
k = 0
for n in nodes:
    if k >= 32:
        break
    eprint("adding ",  n.get('ip'), " (", n.get('udp'), n.get('tcp'), ") by", n.get(
        'maintainer') + ', ' + n.get('location'), "version: ", n.get('version'))

    f.write("    /* by " + n.get('maintainer') +
            ', ' + n.get('location') + " */\n")
    f.write('    { "' + n.get('ip') + '", ' + n.get('ipv6') + ', ' +
            n.get('udp').__str__() + ', ' + n.get('tcp').__str__() + ",\n")
    f.write("        {")
    i = 0
    for p in chunkstring(n.get('pubkey'), 2):
        i += 1
        if i > 16:
            f.write("\n         ")
            i = 0
        f.write(" 0x" + p + ",")

    f.write(" }\n    },\n")
    k += 1

f.write("""
};
#endif
""")

f.close()
eprint("added", k, "nodes.")
