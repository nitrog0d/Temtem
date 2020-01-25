using System;

namespace TemtemCheat {
    class Logger {

        public static void LogLine(string message) {
            Console.WriteLine($"[{DateTime.Now.ToLocalTime()}] {message}");
        }

        public static void LogLine(string message, ConsoleColor color) {
            Console.ForegroundColor = color;
            LogLine(message);
            Console.ResetColor();
        }

        public static void Log(string message) {
            Console.Write($"[{DateTime.Now.ToLocalTime()}] {message}");
        }

        public static void Log(string message, ConsoleColor color) {
            Console.ForegroundColor = color;
            Log(message);
            Console.ResetColor();
        }

    }
}
