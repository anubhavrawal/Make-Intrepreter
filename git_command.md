# Basic Git Command List

## First time Git setup

Tell the Git who you are

`$ git config --global user.name "John Doe" `

`$ git config --global user.email johndoe@example.com `

## Pushing your assignment

Remember to clean all the binary files and the files before you proceed with this. Makefiles's `make clean` should do if you have set it up correctly.

**Step 1.** Stage all the tracked changes

` $git add * `

it might prompt you with the following if you have files or folders in your directory that are mentioned within the `.gitignore ` file.

```
The following paths are ignored by one of your .gitignore files:
<file_name>
<folder_name>
Use -f if you really want to add them.

```
**It's not a error message just warning** 

Remove these file if you wish or just ignore and continue if this does not bother you. 

**Step 2.** Commit these changes with a message explaining the reason for the submitting the assignment this could be basic fix for resubmisssion details.

` $git commit -am "Version 0.1" -am "Reason for the commit" `

**Step 3.** Push all these changes

` $git push `


## For saving user credentials

Tired of entering your username and password each time you say push? Save your credentials so that git remember who you are.

Before you say push enter the following command so that git locally store your username and password.

`$git config --global credential.helper store`


## Other helpful commands

### Discard all changes since last commit

` $git checkout -f `


### Find out why you committed the last time

Must follow the above to steps for this to be relavent

` $git log `

### For untracking a folder:

Step 1. Add the folder path to your repo's root .gitignore file. 

`path_to_your_folder/ `

Step 2. Remove the folder from your local git tracking, but keep it on your disk.

` $git rm -r --cached path_to_your_folder/ `

Step 3. Push your changes to your git repo.


### Remove untacked files

1. First know what you will be deleating: ` $ git clean -nfd `

1. Then clean the it if all looks good: `$ git clean -fd `