using System;
using System.Reflection;
using System.Collections;
using System.Linq;
using Harmony;
using Temtem.Network;
using Sfs2X;
using Sfs2X.Requests;
using Temtem.Core;
using CodeStage.AntiCheat.ObscuredTypes;
using Steamworks;
using Temtem.Cosmetics;
using UnityEngine;
using Sfs2X.Bitswarm;
using Sfs2X.Core;
using Sfs2X.Entities.Data;
using Sfs2X.Controllers;
using temtem.networkserialized;
using Temtem.World;

namespace TemtemCheat.Cheat {
    class TemtemCheat : MonoBehaviour {

        private void HandleLog(string logString, string stackTrace, LogType type) {
            string prefix = "";
            ConsoleColor color = ConsoleColor.Gray;
            switch (type) {
                case LogType.Log:
                    color = ConsoleColor.Green;
                    prefix = "Info";
                    break;
                case LogType.Warning:
                    color = ConsoleColor.Yellow;
                    prefix = "Warning";
                    break;
                case LogType.Error:
                    color = ConsoleColor.Red;
                    prefix = "Error";
                    break;
                case LogType.Assert:
                    color = ConsoleColor.Yellow;
                    prefix = "Assert";
                    break;
                case LogType.Exception:
                    color = ConsoleColor.Red;
                    prefix = "Exception";
                    break;
            }
            Logger.LogLine($"[{Application.productName}] [{prefix}] {logString}", color);
        }

        private void Awake() {
            Application.logMessageReceived += HandleLog;

            Logger.LogLine("Starting PatchManager.");
            PatchManager.Patch();

            

            Logger.LogLine("Starting GUIManager.");
            CheatLoader.cheat.AddComponent<GUIManager>();
        }
    }
}
