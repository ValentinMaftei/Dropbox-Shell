[Română :romania:](#dropbox-shell-ro)

[English :eu:](#dropbox-shell-eu)

# Dropbox Shell RO

##### Proiectat in C, utilizând concepte de sisteme de operare (procese, thread-uri, etc).

Proiectul este o carcasă care expune funcționalitatea utilitarului dbxcli (de exemplu cp, mv, mkdir, rm) și se conectează la sistemul de fișiere local. A fost dezvoltat de mine și [Daria Clem](https://github.com/DariaClem). Este în esență un încapsulare a bibliotecii dbxcli, care poate fi găsită la https://github.com/dropbox/dbxcli.

Am îmbunătățit funcționalitatea dbxcli prin adăugarea posibilității de a încărca și descărca întregi directoare, nu doar fișiere individuale. În plus, am implementat comanda "cd" pentru a face navigarea prin directoarele la distanță mai ușoară, o funcționalitate care nu a fost prezentă în dbxcli original. Comenzile care pot fi utilizate în cadrul programului pot fi consultate utilizând comanda "help".

Carcasa are un design simplu și comenzile sunt evidențiate printr-o paletă de culori similară cu cea a terminalului Linux. În ansamblu, proiectul nostru face lucrul cu funcționalitatea Dropbox prin linia de comandă mai eficient și mai prietenos pentru utilizator.

Pentru a rula programul se folosește:
```
gcc dropboxShell.c -o dropboxShell
./dropboxShell
```

# Dropbox Shell EU

##### Designed in C, using operating systems concepts (processes, threads, etc).

The project is a shell that exposes the functionality of the dbxcli utility (ex. cp, mv, mkdir, rm) and connects with the local file system. It was developed by myself and [Daria Clem](https://github.com/DariaClem). It is essentially a wrapper around the dbxcli library, which can be found at https://github.com/dropbox/dbxcli.

We have enhanced the functionality of the dbxcli by adding the ability to upload and download entire folders, not just individual files. Additionally, we have implemented the "cd" command to make navigation through remote folders easier, a functionality that was not present in the original dbxcli. The commands that can be used within the program can be consulted using the "help" command.

The shell has a simple design and the commands are highlighted by a similar color palette to the Linux terminal. Overall, our project makes working with Dropbox's functionality through the command line more efficient and user-friendly.

To run the program use:
```
gcc dropboxShell.c -o dropboxShell
./dropboxShell
```
