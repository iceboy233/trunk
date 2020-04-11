from __future__ import print_function

import binascii
import json
import os.path
import sys

key_path = sys.argv[1]
key_path_basename = os.path.basename(key_path)
key_h_path = os.path.join(sys.argv[2], key_path_basename + '.h')
key_cc_path = os.path.join(sys.argv[2], key_path_basename + '.cc')

with open(key_path, 'r') as f:
    obj = json.load(f)
    package = obj['package']
    name = obj['name']
    key = binascii.unhexlify(obj['key'])
    if len(key) != 32:
        print('invalid key size', len(key), file=sys.stderr)
        sys.exit(1)

if sys.version_info >= (3,):
    key_source = ''.join('0x{:02x}, '.format(b) for b in key)
else:
    key_source = ''.join('0x{:02x}, '.format(ord(b)) for b in key)

with open(key_h_path, 'w') as f:
    print('#include "security/key.h"', file=f)
    print(file=f)
    for p in package:
        print('namespace {} {{'.format(p), file=f)
    print('extern const ::security::Key {};'.format(name), file=f)
    for p in package:
        print('}} // namespace {}'.format(p), file=f)

with open(key_cc_path, 'w') as f:
    print('#include "{}.h"'.format(key_path_basename), file=f)
    print(file=f)
    for p in package:
        print('namespace {} {{'.format(p), file=f)
    print(
        'const ::security::Key {}{{{{{{{}}}}}}};'.format(name, key_source),
        file=f)
    for p in package:
        print('}} // namespace {}'.format(p), file=f)
