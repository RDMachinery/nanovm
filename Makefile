nanovm: src/nanovm.c src/nanoasm.c 
	gcc src/nanovm.c -o nanovm -Isrc/
	gcc src/nanoasm.c -o nanoasm -Isrc/

clean:
	rm nanoasm.exe
	rm nanovm.exe
	
