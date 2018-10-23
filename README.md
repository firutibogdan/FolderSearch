# FolderSearch

This is a program that works on Windows. It recursively searches in a given path for files with a specified type of extension. Then it opens every file and searches for all lines that contain a given word, and lets all the resulted lines in created file.


Examples of running:


FolderSearch "C:\windows" "txt;exe" "some text" "C:\result"

Check for "some text" in files from "C:\windows" that have the extension "txt" or "exe" and let the result in "C:\result".


FolderSearch "C:\windows" "*" "some text" "C:\result"

Same thing, but check in all files. "*" means everything.
