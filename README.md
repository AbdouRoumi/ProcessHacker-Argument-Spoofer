# ProcessHacker-Argument-Spoofer

## [![Typing SVG](https://readme-typing-svg.demolab.com?font=JetBrains+Mono&weight=2000&pause=1000&width=435&lines=Welcome+to+ProcessHacker-Argument-Spoofer+Repo!!!;Explore+Windows+Monitors+Evasion+Techniques;Educational+&+Research+purposes+only!)](https://git.io/typing-svg)


### ⚖️ **Difference from Procmon Spoofer**
The use of `PEB->ProcessParameters.CommandLine.Buffer` to overwrite the payload can be exposed by **Process Hacker** and tools like **Process Explorer** because these tools use `NtQueryInformationProcess` to read the command-line arguments of a process at runtime. Since this occurs dynamically, they can detect what is currently inside `PEB->ProcessParameters.CommandLine.Buffer`.

In contrast, **Procmon Spoofer** operates differently and focuses on evading tools that primarily rely on static snapshots of process arguments.


## 📚 **Description**
`ProcessHacker-Argument-Spoofer` is a malware evasion technique designed to **manipulate process command-line arguments** to hide the actual payload being executed. This technique is commonly used to **bypass Windows monitoring tools** by spoofing the visible arguments of a process.

- **Startup Argument:** Displayed when the process is created.
- **Real Argument:** The actual command executed after manipulation.

**MITRE ATT&CK ID:** [T1036.005](https://attack.mitre.org/techniques/T1036/005/) *(Masquerading: Match Legitimate Name or Location)*

---


## ⚙️ **How It Works**
1. A target process (e.g., `powershell.exe`) is created in a suspended state with fake command-line arguments.
2. The **Process Environment Block (PEB)** is accessed to locate the command-line arguments in memory.
3. The fake arguments are replaced with the real arguments using `WriteProcessMemory`.
4. The process is resumed and executes the intended payload.

---

## 🛠️ **Technical Details**
- **Fake Argument:** `powershell.exe HELLO WORLD`
- **Real Argument:** `powershell.exe -NoExit calc.exe`
- Uses `NtQueryInformationProcess` to retrieve the PEB of the target process.
- Manipulates `RTL_USER_PROCESS_PARAMETERS` to change the visible arguments.

---

## 🚀 **Usage**
1. Clone the repository:
   ```bash
   git clone https://github.com/your-repo/ProcessHacker-Argument-Spoofer.git
   cd ProcessHacker-Argument-Spoofer
   ```
2. Build the project using Visual Studio.
3. Run the executable:
   ```cmd
   ProcessHacker-Argument-Spoofer.exe
   ```
4. Monitor the process using tools like **Process Explorer** or **ProcMon**.

---

## 🛡️ **Disclaimer**
This tool is intended for **educational and research purposes only**. Unauthorized use is strictly prohibited.

---

## 📄 **License**
This project is licensed under the **MIT License**.

---

## 🤝 **Contributing**
Contributions are welcome! Please submit a pull request or open an issue.

---
For detailed explanations, refer to the source code and documentation.
