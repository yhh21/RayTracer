#! python3
# -*- coding: utf-8 -*-  

import os
import sys
import time
# pip install pillow
from PIL import Image
import DifferrenceManager

if len(sys.argv) < 2 :
    print("len(sys.argv) < 2")
    exit()


SCRIPT_DIR = os.path.dirname(sys.argv[0])

RES_FORM_LOWER = sys.argv[1].lower()
RES_FORM_UPPER = RES_FORM_LOWER.upper()

DES_FORM_LOWER = sys.argv[2].lower()
DES_FORM_UPPER = DES_FORM_LOWER.upper()


def TransPictures() :
    differMgr = DifferrenceManager.CDifferrenceManager()
    
    index = 1
    for dirPath, dirNames, fileNames in os.walk("./" + RES_FORM_UPPER + "/") :
        for fileName in fileNames :
            if fileName.endswith("." + RES_FORM_LOWER) :
                RES_PATH = os.path.abspath(dirPath + fileName)
                desFileName = fileName.replace("." + RES_FORM_LOWER, "." + DES_FORM_LOWER, 1)
                DES_PATH = os.path.abspath("./" + DES_FORM_UPPER + "/" + desFileName)
                if differMgr.IsChangedByPath(RES_PATH) or differMgr.IsChangedByPath(DES_PATH) :
                    img = Image.open(RES_PATH)
                    img.save(DES_PATH)
                    differMgr.IsChangedByPath(DES_PATH)

                    print(str(index) + " : " + fileName + " to " + desFileName)
                    index = index + 1

    differMgr.SaveMD5()


def Main() :
    print("Start Time : " + str(time.ctime()))

    TransPictures()

    print("End Time : " + str(time.ctime()))

if __name__ == '__main__': 
    Main()
