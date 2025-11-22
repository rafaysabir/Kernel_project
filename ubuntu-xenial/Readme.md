COM 341, Operating Systems
==========================
# Project #2: [Process Control Block](https://elixir.bootlin.com/linux/v6.8/source/include/linux/sched.h#L748)

![Compiling a Linux Kernel](http://i.imgur.com/t0c7Wav.gif)

## General Information

In Project #2, you will have to add two system calls to the Linux kernel. The system calls will allow a small task information utility to run in the user space querying information directly from the kernel without parsing output from the `proc` file system.

## Notes

Consider working in a terminal multiplexer such as `tmux`. In `tmux`, you can disconnect from the machine during a long-running process and go back to check the progress of the task at any later time.

Install a terminal multiplexer

    sudo apt install tmux

Type the following to start a new virtual terminal on your remote server

    tmux

To split the current pane into two vertically

    <CTRL+B> "

To split the current pane into two horizontally

    <CTRL+B> %

To switch to the pane above, below, to the left, or to the right of the current pane

    <CTRL+B><Up>
    <CTRL+B><Down>
    <CTRL+B><Left>
    <CTRL+B><Right>

To detach from a virtual terminal

    <CTRL+B><D>

To attach back

    tmux a

To exit from a virtual terminal

    exit

## Steps

1. Download and install [Oracle VirtualBox](https://www.virtualbox.org/wiki/Downloads) for your operating system. It's okay to use your system's package manager. During installation, use the default options if prompted. If you don't have a powerful machine, consider using our lab computers; VirtualBox is already installed on them for you to use.

2. Download an installation image for [Ubuntu 24.04](https://releases.ubuntu.com/noble). We recommend to select the server image under the name `ubuntu-24.04.*-live-server-amd64.iso`. If you have an ARM cpu, you MUST get an ARM image. You can find it [here](https://cdimage.ubuntu.com/releases/24.04.3/release/). We recommend to select the server image under the name `ubuntu-24.04.*-live-server-arm64.iso`. On x86-64, you can also try a desktop image with a GUI. VirtualBox does a decent job of accelerating the GPU to handle the GUI. It's not a bad idea to see how the desktop environment differs from the one you're accustomed to on your host OS. On ARM, VirtualBox's graphics acceleration may be less stable, though you can try running a desktop image there as well if you find one.

3. Open Oracle VirtualBox and open a new machine creation dialog by clicking on the `New` button on the toolbar. We recommend to create the machine in Basic mode, but configure network in Expert mode. VirtualBox hides certain UI elements in basic mode.

4. Enter a descriptive name in the `VM Name` field; in the `VM Folder` field, choose a directory to store the machine (ensure the location has at least 64 GB available); in the `ISO Image` field, select the disk image downloaded in step #1; uncheck `Proceed with Unattended Installation`; set the `OS` field to `Linux`, `OS Distribution` to `Ubuntu`, and `OS Version` to `Ubuntu 24.04 ... (... 64-bit)`. Click on the `Next` button.

5. On the next page, set `Base Memory` to at least 2048 MB. Set `Number of CPUs` to at least 2. Set `Disk Size` to at least 64 GB. We strongly recommend allocating as much as your host system and hardware can spare to speed up compilation; however, be careful not to starve the host OS. When finished, proceed to the next page.

6. Verify your settings and click `Finish`.

7. Right-click your new machine and select `Settings...`. Switch to `Expert` mode.

8. Go to the `Network` tab, open the `Port Forwarding` window. Click on the plus icon and add the following rule to forward traffic from your host machine on port 2222 to an SSH server on the guest machine on port 22.

        Name      Protocol    Host IP      Host Port    Guest IP     Guest Port
        Rule 1    TCP         127.0.0.1    2222         10.0.2.15    22

    Other options are possible, but they depend on your network setup (e.g., you can select the `Bridged Adapter` in the `Attached to` drop-down as an alternative).

9. Save your settings and use the `Start` button on the toolbar to start your virtual machine.

10. In the GRUB bootloader, select `Try or Install Ubuntu Server`.

11. On the language selection dialog, set the system language to `English`.

12. On the keyboard layout selection dialog, set `English (US)`.

13. In the OS installation type dialog, select `Ubuntu Server`.

14. On the network configuration dialog, leave the default values as is.

15. Set proxy parameters. Ignore the dialog if you don't have a proxy server.

16. On the mirror configuration page, ensure that you have `http://archive.ubuntu.com/ubuntu/`.

17. On the disk storage layout screen, ensure that `Use an entire disk` is selected and proceed.

18. Aknowledge the file system summary and confirm destructive operations.

19. Enter your full name. You can pick any full name or login name this time. Set a system hostname (server name) to identify the virtual machine on your network (e.g., `ubuntu-24-04`). Select a login name and create a password for a new account on your virtual system. The login name and password can be anything you want this time. Make sure to remember this information. Move on to the next page.

20. Skip the Ubuntu Pro upgrade.

21. Check `Install OpenSSH server`, ignore all the other options, and proceed.

22. Do not install any Ubuntu snaps. If you continue, the installation process will start.

23. Reboot the virtual machine at the end of the installation process. When asked, remove the disk image by clicking on a disk icon in the lower right corner and selecting `Remove disk from virtual drive`. It is also possible that the OS will remove the disk automatically for you. Press `Enter` if the system is not rebooting. If that doesn't help, open the `Machine` menu and choose `Reset...`.

24. It's a good idea to create a snapshot of the virtual machine so you can return to a clean state if something goes wrong. To do that, shut down the machine from the `Machine` menu and choose `Shut Down...`. Then open the `Snapshot` menu and click `Take...`. Pick a sensible name for the snapshot. You can start the machine again. Consider making snapshots after every major accomplishment so you can restore the machine if something goes wrong.

25. Connect to your server through the SSH protocol.

    On your host machine (from Bash on Windows or any terminal on *nix platforms), use the following command to connect to your virtual server.

        ssh -p 2222 <the user name specified during installation>@127.0.0.1

    Consider starting using `tmux` at this point. See the notes above for more information about `tmux`.

26. Make Ubuntu's volume manager use the whole disk. For some reason, only the first 32 GB half is mapped.

        sudo pvresize /dev/sda3
        sudo lvresize -l +100%FREE /dev/mapper/ubuntu--vg-ubuntu--lv
        sudo resize2fs /dev/mapper/ubuntu--vg-ubuntu--lv

27. Make the package manager to allow downloading package sources by adding the `deb-src` text after the lines `Types: deb` in the `/etc/apt/sources.list.d/ubuntu.sources` file.

        sudo vim /etc/apt/sources.list.d/ubuntu.sources

    Change the first line of the block

        Types: deb
        URIs: http://ports.ubuntu.com/ubuntu-ports/
        Suites: noble noble-updates noble-backports
        Components: main restricted universe multiverse
        Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

    to

        Types: deb deb-src
        URIs: http://ports.ubuntu.com/ubuntu-ports/
        Suites: noble noble-updates noble-backports
        Components: main restricted universe multiverse
        Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

    Change the first line of the next block

        Types: deb
        URIs: http://ports.ubuntu.com/ubuntu-ports/
        Suites: noble-security
        Components: main restricted universe multiverse
        Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

    to

        Types: deb deb-src
        URIs: http://ports.ubuntu.com/ubuntu-ports/
        Suites: noble-security
        Components: main restricted universe multiverse
        Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

28. Update package index and install updates to the system. Reboot the machine if a new kernel was installed.

        sudo apt update && sudo apt upgrade

29. Install `git`, `make`, `gcc`, and `ncurses-dev` if they are not installed.

        sudo apt install git make gcc ncurses-dev

30. Upload the `project-2-*` directory to the virtual machine from the host environment or clone the repository from the GitHub in the Guest environment.

        # From your Host machine
        scp -P 2222 -r project-2-* <your login name>@127.0.0.1:~/

    or

        # From your Guest machine
        git clone <the URL of the repository generated by the GitHub Classrom system>

31. Go to the directory with sources of the process information utility `tasks`.

        cd ~/project-2-*/tasks

32. Compile the user space program.

        make

33. Ensure that it doesn't work.

        ./tasks

34. Go to your home directory.

        cd ~/

35. Set your name and e-mail address in Git if you haven't done it yet.

        git config --global user.name "your full name"
        git config --global user.email "your e-mail address"

36. Get the Linux kernel sources for Ubuntu 24.04.

        sudo apt install dpkg-dev
        apt source linux-image-unsigned-$(uname -r)

37. Install prerequisites for building the kernel.

        sudo apt build-dep linux linux-image-unsigned-$(uname -r)
        sudo apt install libncurses-dev gawk flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf llvm linux-tools-common fakeroot

38. Go inside the kernel source tree.

        cd linux-*/

39. Copy system call entry points into the kernel source tree.

        cp -R ~/project-2-*/ubuntu-xenial/task_info ./

40. Add a kernel configuration entry to disable or enable the new subsystem.

        vim ./init/Kconfig

    Add the following

        config TASK_INFO
        	bool "task_info syscalls"
        	default y
        	help
        	  This option enables the task_info system calls which can be
        	  used to query task information directly from the kernel.

    at the end of the file. Adjust tabs and spaces.

41. Enable the system only for `amd64` and `arm64` system in the `./debian.master/config/annotations` file.

        vim ./debian.master/config/annotations

    Add the following

        CONFIG_TASK_INFO                                policy<{'amd64': 'y', 'arm64': 'y', 'armhf': 'n', 'ppc64el': 'n', 'riscv64': 'n', 's390x': 'n'}>

    at the end of the file. Adjust tabs and spaces.

42. In the same file, remove all the flavors that we are not going to build for. If you have an x86-64 machine, you can remove all the flavors except `amd64`. If you have an ARM machine (Apple Silicon Chips), you can remove all the flavors except `arm64`.

        vim ./debian.master/config/annotations

    Adjust the comment line

        # FLAVOUR: amd64-generic arm64-generic arm64-generic-64k armhf-generic ppc64el-generic riscv64-generic s390x-generic

    by deleting all the flavors that we are not going to build for. For example, if you have an x86-64 machine, you can remove all the flavors except `amd64`.

        # FLAVOUR: amd64-generic

    On ARM machines, you can remove all the flavors except `arm64`.

        # FLAVOUR: arm64-generic

    You can find the architecture of your machine by running the following command

        uname -a

43. If you are on an ARM64 architecture, you need to adjust the `debian.master/rules.d/arm64.mk` file. Remove the `flavours = generic-64k` line from the file. On AMD64, you can skip this step.

        vim ./debian.master/rules.d/arm64.mk

    Remove `generic-64k` from the line

        flavours        = generic generic-64k

    It should look like

        flavours        = generic

    at the end.

44. Add a forward declaration for the `task_info` structure and two function prototypes for `get_pids` and `get_task_info` at the end of `include/linux/syscalls.h`.

        vim ./include/linux/syscalls.h

    Add

        struct task_info;

    after

        struct mnt_id_req;

    Add the following

        /* task_info */
        asmlinkage long sys_get_pids(int pid_list_length, pid_t __user *output_pid_list);
        asmlinkage long sys_get_task_info(pid_t pid, struct task_info __user *output_task_info);

    after the line

        asmlinkage long sys_lsm_list_modules(u64 *ids, u32 *size, u32 flags);

45. Add generic table entries for two system calls before the first instance of `#undef __NR_syscalls` in `include/uapi/asm-generic/unistd.h`. Do not forget to increment the counter for the first `#define __NR_syscalls` by two.

        vim ./include/uapi/asm-generic/unistd.h

    Add

        /* task_info/get_pids.c */
        #define __NR_get_pids 462
        __SYSCALL(__NR_get_pids, sys_get_pids)
        /* task_info/get_task_info.c */
        #define __NR_get_task_info 463
        __SYSCALL(__NR_get_task_info, sys_get_task_info)

    before

        #undef __NR_syscalls

    Don't forget to adjust numbers `462` and `463` if necessary. Use successive values after the last system call in the file.

    Increment the value of the next instance of `__NR_syscalls` by two.

    For example, change

        #define __NR_syscalls 462

    to

        #define __NR_syscalls 464

46. Add x86 table entries for two system calls at the end of the `arch/x86/entry/syscalls/syscall_32.tbl` file.

        vim ./arch/x86/entry/syscalls/syscall_32.tbl

    Add the following

        462	i386	get_pids		sys_get_pids
        463	i386	get_task_info		sys_get_task_info

    at the end of the file. Adjust tabs and spaces.

    Don't forget to adjust numbers `462` and `463` if necessary. Use successive values after the last system call in the file.

47. Add x86-64 table entries for two system calls in `arch/x86/entry/syscalls/syscall_64.tbl`. Do it before the `# Due to a historical design error...` comment.

        vim ./arch/x86/entry/syscalls/syscall_64.tbl

    Add system call numbers for x86-64 architecture

        462	common	get_pids		sys_get_pids
        463	common	get_task_info		sys_get_task_info

    before

        #
        # Due to a historical design error, certain syscalls are numbered differently...

    Don't forget to adjust numbers `462` and `463` if necessary. Use successive values after the last system call in the file. Adjust tabs and spaces appropriately.

48. Add two fallback stubs at the end of `kernel/sys_ni.c`.

        vim ./kernel/sys_ni.c

    Add the following

        /* task_info */
        COND_SYSCALL(sys_get_pids);
        COND_SYSCALL(sys_get_task_info);

    after the line

        COND_SYSCALL(lsm_list_modules);

49. Modify the kernel build system to compile the new calls. Add the directory `task_info/` after the line `obj-y += kernel/` in the `Kbuild` file.

        vim ./Kbuild

    Add the line

        obj-y			+= task_info/

    after the line

        obj-y			+= kernel/

50. Append a plus sign followed by your full last name and the first letter of your first name (e.g., `+toksaitovd`) to the version number at the top of `debian.master/changelog` to identify your new kernel. Use lowercase Latin letters only. Do not insert any other characters between the plus sign and the letters. Your work will be graded based on the correctness of this value. Incorrect values will result in zero points. Ensure that you specify your name correctly.

        vim ./debian.master/changelog

    Change the version at the top (the version can be different)

        linux (6.8.0-86.87) noble; urgency=medium

    to

        linux (6.8.0-86.87+toksaitovd) noble; urgency=medium

    Do not forget to adjust the login name appropriately.

51. Make Debian build scripts executable.

        chmod a+x debian/rules
        chmod a+x debian/scripts/*
        chmod a+x debian/scripts/misc/*

52. Clean the build directory.

        fakeroot debian/rules clean

53. Create default configuration files for the kernel.

        fakeroot debian/rules editconfigs

    For each prompt, ensure that the `task_info` option is selected, save the configuration and exit. Ignore any error for architectures that we are not building for. You can also disable generation of debug information to speed up the compilation process and minimize the size of the resulting kernel. Various drivers and subsystems that we don't need such as sound and graphics can also be disabled. Do this if you have a limited amount of time to compile the kernel or a limited amount of disk space. Note that disabling some drivers may make your system unbootable.

54. Start the kernel build process. A successful build will produce multiple
    `.deb` packages in the parent directory. Time the process.

        time fakeroot debian/rules binary-headers binary-generic binary-perarch

    The compilation process can take a lot of time. Compiled objects can use up to 40 gigabytes of disk space. The Debian build system not only builds the kernel but also packs everything into a set of installable `.deb` packages. All compilation errors will appear at this stage.

    Keep track of errors by watching the compilation log. The compilation process does not stop on error. Stop the process immediately if you see an error by pressing `CTRL+C` key combination.

    To restart an unsuccessful build, fix problems in your sources and start the build system again.

        time fakeroot debian/rules binary-headers binary-generic binary-perarch

    To restart just the unsuccessful kernel build, fix problems in your sources and start the build system again in the following way

        time fakeroot debian/rules binary-generic

55. The first problem you will encounter is a complaint that the variable `nr_threads` is undeclared in `get_pids.c`. Use the Elixir Linux Cross Reference for Linux kernel version [6.8.\*](https://elixir.bootlin.com/linux/v6.8/source) to search where `nr_threads` is declared. You should also find where the same variable was included for the [old version](https://elixir.bootlin.com/linux/v4.8/source) of the kernel used in Ubuntu 16.10. Remove the old included header (if necessary) and replace it with the new one in `get_pids.c`. Note, that the file that you include must be an `.h` file. The variable should also be declared as an `externvar` and not as a member in the header.

56. The second problem is that `linux/cputime.h` does not exist in the 6.8.* version of the kernel anymore that is included in `get_task_info.c`. To find where `cputime.h` code was moved into, search in the commit history of the Linux [kernel](https://github.com/torvalds/linux). I recommend using the following string `Move cputime functionality from` to narrow down your search. Fix the include after that. It also looks like that the `cputime_t` was removed and replaced with the `u64` typedef. Replace the type in `get_task_info.c`.

57. The third problem is that `cputime_to_usecs` is not used to convert time values in the new version of the kernel anymore. To help us find what should we use, we will refer to the Linux Cross Reference [again](https://elixir.bootlin.com/linux/v4.8/source). Find the `cputime_to_usecs` usage in 4.8 sources. Compare the same places in sources in the [6.8.](https://elixir.bootlin.com/linux/v6.8/source) version on the same site. Apply the changes to your code.

58. Finally, the logic to extract the process state information (outlined after the `/* state */` comment in `get_task_info.c`) was changed, and a getter `task_state_index(...)` was [created](https://github.com/torvalds/linux/commit/1d48b080bcce0a5e7d7aa2dbcdb35deefc188c3f) to acquire this information from the process control block (`struct task_struct` in Linux). Replace all code associated with the `/* state */` comment with a getter call to initialize the `local_task_info.state` field.

59. Fix the code issues and try rebuilding the kernel again.

60. Go to the parent directory.

        cd ..

61. Ensure that you have a number of newly created `.deb` packages.

        ls *.deb

62. In VirtualBox, create a new snapshot of your virtual machine. If something goes wrong during the project, you can restore the machine to its current state from the same menu.

63. Install the new kernel and all its supporting files.

        sudo dpkg -i *.deb

    In case of dependency errors, run the following

        sudo apt -f install

    and restart the installation process

        sudo dpkg -i *.deb

    If the package manager complains about the conflict with the existing kernel, remove the old kernel and install the new one.

        sudo apt remove linux-image-6.8.0-86-generic # or whatever version `uname -a` reports
        sudo apt autoremove
        sudo dpkg -i *.deb

64. Reboot the machine to start using the new kernel.

        sudo shutdown -r now

65. Reconnect to your machine with SSH.

66. Go to the directory with sources of the process information utility "tasks".

        cd ~/project-*/tasks

67. Adjust the system call numbers that you have defined in the kernel. Change
    values for constants `__NR_get_pids` and `__NR_get_task_info` in `tasks.c`.

        vim tasks.c

68. Recompile the user space program.

        make clean
        make

69. Test the new system calls.

        ./tasks

    You should see a list of tasks currently running on the system. If you still see an error complaining that the `task_info` subsystem is missing, you most likely made a mistake in one of the previous steps. Use your best judgment to decide where to restart the process.

70. Go back to your project's Git repository.

### Submitting Work

In your project's Git repository, add the following files to the `ubuntu-noble` directory:

* `ubuntu-noble/task_info/get_pids.c`
* `ubuntu-noble/task_info/get_task_info.c`
* `ubuntu-noble/vmlinuz-6.8.0-*-generic`

Figure out on your own on where to find and how to extract the last file from your `*.deb` packages. Create all the necessary directories and subdirectories to match the structure of the files in the repository.

Commit and push your changes with an appropriate commit message. Then check the GitHub Actions page to confirm that your commit passes the grader checks and that a boot-process video has been recorded by the grader. Review the video of your kernel booting. At the end of the video, verify that the `tasks` program is running.

Submit a URL that points to a successful commit to Moodle. Ensure that the URL does not point to the GitHub Actions page.

## Deadlines

Refer to Moodle for information about the deadline.

## Additional Information

### Web Resources

* [Linux Cross Reference](https://elixir.bootlin.com)

* [Linux Documentation, Adding a New System Call](https://github.com/torvalds/linux/blob/6f0d349d922ba44e4348a17a78ea51b7135965b1/Documentation/process/adding-syscalls.rst)

* [Linux Filesystem Hierarchy, /proc](http://www.tldp.org/LDP/Linux-Filesystem-Hierarchy/html/proc.html)

### Documentation

    man tmux

    man 2 syscall
    man 2 syscalls
    man 2 sysinfo
    man 5 proc

### Books

* _Understanding the Linux kernel, Third Edition by Daniel P. Bovet and Marco Cesati, Chapters 3, 4, 7, 10_

* _Linux Kernel Development, Third Edition by Robert Love, Chapters 3-5, 7_
