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
**It's not a error message just a warning** 

Remove these file if you wish or just ignore and continue if this does not bother you. 

**Step 2.** Commit these changes with a message explaining the reason for the submitting the assignment this could be basic fix for resubmission details.

` $git commit -am "Version 0.1" -am "Reason for the commit" `

**Step 3.** Push all these changes

` $git push `

> Note always go for `git pull` before staging changes when collaborating with multiple people on the same repo.

## For saving user credentials

Tired of entering your username and password each time you push your changes? Lets use the SSH encryption for saving and accessing your repository.

Lets begin for generating your customized SSH encryption key

`ssh-keygen -t rsa -C "johndoe@example.com" `


For ideal results:

```
$ssh-keygen -t rsa -C "johndoe@example.com
Generating public/private rsa key pair.
Enter file in which to save the key (/home/<your_username>/.ssh/id_rsa): /<your_username>/.ssh/id_rsa

Enter passphrase (empty for no passphrase): <enter a passphrase with at least 8 characters>
```
Go to your GitHub account settings and select `SSH and GPG keys` tab and select `New SSH key` button. Give a title you wish and copy the content of the file `~/.ssh/id_rsa.pub` to the key section.

Moving back to your terminal, lets test the connection with 

`ssh -T git@github.com`

the above command will process, type `yes` when prompted and then it will ask for your **passphrase if you had setup one when it was prompted last time**. Enter the **passphrase** and the execution should look like bellow:

```
$ ssh -T git@github.com
The authenticity of host 'github.com (140.82.114.4)' can't be established.
RSA key fingerprint is SHA256:<your_key>.
Are you sure you want to continue connecting (yes/no)? yes
Warning: Permanently added 'github.com,140.82.114.4' (RSA) to the list of known hosts.
Hi <git username>! You've successfully authenticated, but GitHub does not provide shell access.
```

Then, `cd` into your cloned repository and enter the command:

`git remote set-url origin git@github.com:wmucs<classcode>/<assignment_repository_name>.git`

Now you have completed the ssh setup for your repository, from now on your `git push` will not prompt form username and password. 

**Note:** Your `git push` might execute as 

```
$ git push
Warning: Permanently added the RSA host key for IP address '140.82.113.4' to the list of known hosts.
Counting objects: 2, done.
Delta compression using up to 4 threads.
Compressing objects: 100% (2/2), done.
Writing objects: 100% (2/2), 220 bytes | 220.00 KiB/s, done.
Total 2 (delta 1), reused 0 (delta 0)
remote: Resolving deltas: 100% (1/1), completed with 1 local object.
To github.com:wmucs3240/<repo-name>.git
   c9398d6..acd1536  master -> master
```

> This execution is as expected and works as expected.

## Saving user credentials 2

SSH too complicated for you??

Save your credentials locally, so that git remember who you are.

Before you say push enter the following command so that git locally store your username and password.

`$git config --global credential.helper store`


## Other helpful commands

### For removing user credentials

`git config --global --unset credential.helper`

### Discard all changes since last commit

` $git checkout -f `


### Find out why you committed the last time

Must follow the above steps for this to be relevant

` $git log `

### For untracking a folder:

Step 1. Add the folder path to your repo's root .gitignore file. 

`path_to_your_folder/ `

Step 2. Remove the folder from your local git tracking, but keep it on your disk.

` $git rm -r --cached path_to_your_folder/ `

Step 3. Push your changes to your git repo.


### Remove untacked files

1. First know what you will be deleting: ` $ git clean -nfd `

1. Then delete it, if all looks good: `$ git clean -fd `