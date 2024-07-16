# Enter-the-VM
This project involves creating a custom hypervisor using KVM (Kernel-based Virtual Machine) in Linux. The project is divided into two main parts: building a DIY hypervisor and creating a fractional scheduling mechanism for multiple VMs.



# Simple KVM Hypercall Extensions

This project extends a simple KVM (Kernel-based Virtual Machine) implementation by adding several hypercalls. These hypercalls allow the Guest OS to perform operations that require hypervisor intervention. The hypercalls are implemented in `simple-kvm.c` and include functions to print values, count VM exits, print strings, and convert guest virtual addresses to host virtual addresses.

## Hypercall Implementations

### 1. `void HC_print32bit(uint32_t val)`

This hypercall takes a 32-bit value as input and prints it on the terminal. The hypercall should ensure a single newline (`\n`) is appended after printing the value.

### 2. `uint32_t HC_numExits()`

This hypercall returns the number of times the guest VM has exited to the hypervisor since the start of its execution. The hypervisor maintains this count. The count is printed using the `HC_print32bit` hypercall.

### 3. `void HC_printStr(char *str)`

This hypercall prints the string pointed to by `str`. The string is fetched from the guest’s memory and printed by the hypervisor. The count of VM exits before and after this hypercall should only increase by one. Use `HC_numExits` to count and verify the exits.

### 4. `char * HC_numExitsByType()`

This hypercall returns an address to a string containing the counts of VM exits due to IO in and IO out operations. The format of the string is:
```
IO in: x
IO out: y
```
The string is printed using the `HC_printStr` hypercall.

### 5. `uint32_t HC_gvaToHva(uint32_t gva)`

This hypercall converts a given Guest Virtual Address (GVA) to a Host Virtual Address (HVA). If the GVA is invalid, the hypervisor prints "Invalid GVA" and returns 0. The resulting HVA is printed using the `HC_print32bit` hypercall. Use the `KVM_TRANSLATE` request flag and `ioctl` with the `vmfd` for this conversion.

## Implementation Details

### Boilerplate Code

The boilerplate code already implements a basic hypercall `HC_print8bit()`, which writes an 8-bit value to the port `0xE9` using the `outb` assembly instruction. The additional hypercalls are built upon this example.

### Usage

1. Clone the repository:
    ```
    git clone https://github.com/your-username/simple-kvm-hypercall-extensions.git
    cd simple-kvm-hypercall-extensions
    ```

2. Compile the hypervisor and guest code:
    ```
    make
    ```

3. Run the KVM with the extended hypercalls:
    ```
    ./simple-kvm
    ```

4. The guest code should make hypercalls, and the hypervisor will handle them according to the implementations described above.

## Examples

### Example of `HC_print32bit`

Guest code:
```c
HC_print32bit(1234567890);
```

Output:
```
1234567890
```

### Example of `HC_numExits`

Guest code:
```c
uint32_t exits = HC_numExits();
HC_print32bit(exits);
```

Output:
```
5
```

### Example of `HC_printStr`

Guest code:
```c
char *msg = "Hello from Guest!";
HC_printStr(msg);
```

Output:
```
Hello from Guest!
```

### Example of `HC_numExitsByType`

Guest code:
```c
char *exitTypes = HC_numExitsByType();
HC_printStr(exitTypes);
```

Output:
```
IO in: 10
IO out: 20
```

### Example of `HC_gvaToHva`

Guest code:
```c
uint32_t hva = HC_gvaToHva(valid_gva);
HC_print32bit(hva);
hva = HC_gvaToHva(invalid_gva);
HC_print32bit(hva);
```

Output:
```
<valid HVA>
Invalid GVA
0
```

## Conclusion

These extensions enhance the interaction between the Guest OS and the hypervisor, providing more functionality and control over VM operations. The implementation demonstrates key concepts in handling hypercalls and VM exits within a KVM environment.


# Matrix Cloud Hypervisor Project

## Overview

The Matrix Cloud Hypervisor project aims to create a custom hypervisor using KVM in Linux. This hypervisor will host two guest operating systems (VMs) belonging to Neo and Morpheus on a single physical CPU core. Neo's VM has a higher scheduling priority, with 70% of the CPU time allocated, while Morpheus gets 30%.

## Project Structure

The project is divided into several key parts:

1. **Matrix.c**
   - This is the main control program acting as the custom KVM hypervisor.
   - It sets up and manages two guest programs: `guest1.s` (Neo's VM) and `guest2.s` (Morpheus's VM).
   - Each VM is configured with one vCPU.

2. **Guest Programs**
   - `guest1.s`: Issues hypercalls via port 0x10, increments the value in `ax` register on return.
   - `guest2.s`: Issues hypercalls via port 0x11, decrements the value in `ax` register on return.
   - Both programs run in an infinite loop.

## Project Tasks

### Task 2a: Alternating VM Execution

Instead of creating pthreads for vCPUs, the `kvm_run_vm` function is modified to use the main thread to alternate between the two VMs on `KVM_EXIT_IO` events.

### Task 2b: Handling IO Operations

To handle situations where no IO operation occurs, timer interrupts are utilized. This involves:
- Using `timer_create()` to set up a per-process interval timer.
- Configuring the timer to issue a signal periodically.
- Using `KVM_SET_SIGNAL_MASK` to manage signals within the KVM execution.

### Task 2c: Fractional Scheduling

Implementing fractional scheduling involves:
- Defining a time quantum (`QUANTUM`) with a value of 1 second.
- Using macros (`FRAC_A` and `FRAC_B`) to schedule Neo's and Morpheus's VMs based on predefined fractions (default: 7/10 for Neo and 3/10 for Morpheus).

## Usage

1. **Setup**
   - Compile `matrix.c` and the guest programs (`guest1.s` and `guest2.s`).
   - Ensure KVM support is enabled in your Linux kernel.

2. **Execution**
   - Run `matrix.c`, which initializes and manages the VMs.
   - Monitor the output and CPU usage to observe the scheduled execution of Neo's and Morpheus's VMs according to the defined fractions.

## Conclusion

The Matrix Cloud Hypervisor project demonstrates the implementation of a custom hypervisor using KVM, focusing on efficient VM scheduling and fractional resource allocation. Future enhancements could include additional VM management features and optimization of scheduling algorithms.


# Directory Structure

```
Hypervisor-Symphony/
├──── .git/
│        └── . . . /* all git-related files */
│
├──── part1b/
│        ├── guest.c
│        ├── guest.ld
│        ├── guest16.s
│        ├── Makefile
│        ├── payload.ld
│        ├── README.md
│        └── simple-kvm.c
│
├──── part2/
│        ├── guest1.s
│        ├── guest1-a.s
│        ├── guest1-b.s
│        ├── guest2.s
│        ├── guest2-a.s
│        ├── guest2-b.s
│        ├── Makefile
│        ├── matrix-a.c
│        ├── matrix-b.c
│        ├── matrix.c
│        └── README.md
│
├──── part1a.pdf
├──── .gitignore
├──── Makefile
└──── README.md
```

# References

- [KVM Paper](https://www.cse.iitb.ac.in/~puru/courses/spring2023-24/downloads/kvm.pdf)
- [Linux KVM API 1](https://lwn.net/Articles/658511/)
- [Linux KVM API 2](https://docs.kernel.org/virt/kvm/api.html)
- [Setup Procedure (for Ubuntu)](https://help.ubuntu.com/community/KVM/Installation)

- https://man7.org/linux/man-pages/man2/timer_create.2.html

- https://docs.kernel.org/virt/kvm/api.html

