#! python3
# -*- coding: utf-8 -*-  

import os
import sys
import json
import codecs
import hashlib
import time

curTime = str(time.ctime())
curTimeInverse = curTime[::-1]

SCRIPT_DIR = os.path.dirname(sys.argv[0])
SAVE_MD5_PATH = SCRIPT_DIR + "\MD5.json"

def Singleton(cls):
    instances = {}

    def getInstance(*args, **kwargs) :
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]

    return getInstance


@Singleton
class CDifferrenceManager() :
    def __init__(self) :
        self.mMD5s = {}
        if os.path.exists(SAVE_MD5_PATH):
            self.mMD5s = json.load(open(str(SAVE_MD5_PATH), "r"))
        self.mNewMD5s = {}

    # bug
    #def __exit__(self) :
    #    SaveMD5()

    def IsChangedByPath(self, filePath) :
        tmpMD5 = self._GetFileMD5ByPath(filePath)

        res = False
        if (filePath in self.mNewMD5s) or (filePath not in self.mMD5s) or (self.mMD5s[filePath] != tmpMD5):
            self.mNewMD5s[filePath] = tmpMD5
            res = True
        return res
    
    def _GetFileMD5ByPath(self, filePath) :
        if not os.path.exists(filePath):
            return curTimeInverse
        
        #md5 = os.path.getmtime(filePath)
        md5_obj = hashlib.md5()

        f = open(filePath, 'rb')
        while True:
            d = f.read(8096)
            if not d:
                break
            md5_obj.update(d)
        f.close()
        
        hash_code = md5_obj.hexdigest()
        md5 = str(hash_code).lower()
        return md5

    def SaveMD5(self) :
        for key in self.mNewMD5s :
            self.mMD5s[key] = self.mNewMD5s[key]

        encodedJson = json.dumps(self.mMD5s, sort_keys = True, indent = 2, ensure_ascii = False)
        f = codecs.open(str(SAVE_MD5_PATH), "wb+", "utf-8")
        f.write(encodedJson)
        f.close()

