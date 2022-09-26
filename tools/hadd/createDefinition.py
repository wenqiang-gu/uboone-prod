#!/usr/bin/env python
# copied from definitions.py
from __future__ import absolute_import
from samweb_client import json, convert_from_unicode, convert_to_string
from samweb_client.client import samweb_method
from samweb_client.http_client import escape_url_path, escape_url_component
from samweb_client.exceptions import *

import os,sys
defname=sys.argv[1]
print(defname)

dims='file_name'
with open("offline_path.txt") as f:
  for line in f:
    if dims=='file_name': dims += ' '
    else: dims += ' ,'
    dims += os.path.basename(line.strip('\n'))
# print(dims)

# copied from samweb_cli.py
from samweb_client import *
samweb = SAMWebClient()

# copied from createDefinition(samweb, defname, dims, user=None, group=None, description=None)
params = { "defname": defname,
         "dims": dims,
         "group": samweb.group,
         }
samweb.postURL('/definitions/create', params, secure=True)
print("create defname: %s" %defname)
print("please check: samweb count-definition-files %s" %defname)
