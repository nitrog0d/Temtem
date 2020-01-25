using System;
using System.Runtime.InteropServices;
using System.IO;
using UnityEngine;

namespace TemtemCheat {

    class ConsoleWindow {
        [DllImport("kernel32.dll")]
        private static extern int AllocConsole();

        [DllImport("user32.dll")]
        [return: MarshalAs(2)]
        private static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetConsoleWindow();

        private static void ShowConsole() {
            SetForegroundWindow(GetConsoleWindow());
        }

        internal static void CreateConsole() {
            AllocConsole();
            Console.SetOut(new StreamWriter(Console.OpenStandardOutput()) { AutoFlush = true });
            Console.SetIn(new StreamReader(Console.OpenStandardInput()));
            Console.Title = $"{Application.productName} v{Application.version} - TemtemCheat - Created by nitro.#0007";
            // ShowConsole();
        }
    }
}