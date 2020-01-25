using UnityEngine;
using System;

namespace TemtemCheat {
    public class CheatLoader {
        public static void Inject() {
            ConsoleWindow.CreateConsole();
            Logger.LogLine("Cheat injected!", ConsoleColor.Green);
            Logger.LogLine($"Game: {Application.productName} v{Application.version}", ConsoleColor.Green);
            cheat = new GameObject();
            cheat.AddComponent<Cheat.TemtemCheat>();
            UnityEngine.Object.DontDestroyOnLoad(cheat);
        }

        public static GameObject cheat;
    }
}
