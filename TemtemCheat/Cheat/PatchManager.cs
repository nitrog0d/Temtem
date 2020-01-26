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
using Temtem.UI;
using TMPro;

namespace TemtemCheat.Cheat {
    class PatchManager {

        public static HarmonyInstance harmony = null;

        private static void ApplyPatch(MethodBase method, string patchMethodName, bool postfix = false) {
            MethodInfo patchMethod = typeof(PatchManager).GetMethod(patchMethodName, BindingFlags.Static | BindingFlags.NonPublic);
            if (method != null && patchMethod != null) {
                string usingPostfix = postfix ? " (Postfix)" : " (Prefix)";
                Logger.LogLine($"[PatchManager] Patching method {method.DeclaringType.Name}/{method.Name} with {patchMethodName}{usingPostfix}.");
                if (postfix) harmony.Patch(method, null, new HarmonyMethod(patchMethod)); else harmony.Patch(method, new HarmonyMethod(patchMethod));
            } else {
                Logger.LogLine($"[PatchManager] Failed to patch {patchMethodName}, method not found.");
            }
        }

        public static void Patch() {
            if (harmony == null) harmony = HarmonyInstance.Create("TemtemCheat");

            // Update queue every 5 seconds
            // https://nitro.moe/zvzyv.png
            MethodInfo queueTimeMethod = typeof(NetworkLogic).GetMethods(BindingFlags.NonPublic | BindingFlags.Instance).Where(x =>
            (x.ReturnType == typeof(void)) &&
            (x.GetParameters().Length == 2) &&
            (x.GetParameters()[0].ParameterType == typeof(float)) &&
            (x.GetParameters()[1].ParameterType == typeof(bool))
            ).ToArray()[0];
            ApplyPatch(queueTimeMethod, "QueueTimePatch");

            // Patch SmartFox
            ApplyPatch(typeof(SFSIOHandler).GetMethod("OnDataWrite", BindingFlags.Public | BindingFlags.Instance), "SFOnDataWritePatch");
            ApplyPatch(typeof(SFSProtocolCodec).GetMethod("DispatchRequest", BindingFlags.NonPublic | BindingFlags.Instance), "SFDispatchRequestPatch");

            // Spawn chance when in wild area
            // https://nitro.moe/kYl5x.png
            ApplyPatch(typeof(WildMonstersLogic).GetMethods(BindingFlags.NonPublic | BindingFlags.Instance).Where(x =>
            (x.ReturnType == typeof(void)) &&
            (x.GetParameters().Length == 1) &&
            (x.GetParameters()[0].ParameterType == typeof(SpawnZoneDefinition))
            ).ToArray()[10], "SpawnChancePatch");

            // Disable obscured types
            ApplyPatch(typeof(GameLogic).GetMethod("Awake", BindingFlags.NonPublic | BindingFlags.Instance), "GLAwakePatch", true);
        }

        private static bool QueueTimePatch(ref float __0) {
            __0 = 5f;
            return true;
        }

        private static bool SpawnChancePatch(WildMonstersLogic __instance, SpawnZoneDefinition __0) {
            if (CheatOptions.instantAggro && !CheatOptions.noAggro) {
                InstanceManager.battleMethod.Invoke(__instance, new object[] { __0 });
            }
            if (CheatOptions.noAggro) {
                return false;
            }
            return true;
        }

        // Request
        private static bool SFOnDataWritePatch(ref IMessage __0) {
            ISFSObject raw = __0.Content;
            bool log = true;
            // System
            if (raw.GetByte("c") == 0) {

            }
            // Game
            else if (raw.GetByte("c") == 1) {
                ISFSObject p = raw.GetSFSObject("p");
                // Player movements
                string c = p.GetUtfString("c");
                if (c == "spawnMonster") {
                    ISFSObject p2 = p.GetSFSObject("p");
                    p2.PutBool("sp", true);
                }
                if (c == "UPP" || c == "playerAction.CM") log = false;
            }
            if (log) Logger.LogLine($"Sending request (ID {Convert.ToInt32(raw.GetShort("a"))}):\n{raw.GetDump()}", ConsoleColor.Green);
            return true;
        }

        // Response
        private static bool SFDispatchRequestPatch(ref ISFSObject __0) {
            ISFSObject raw = __0;
            bool log = true;
            // System
            if (raw.GetByte("c") == 0) {
                ISFSObject p = raw.GetSFSObject("p");
                // Player movements
                int a = Convert.ToInt32(raw.GetShort("a"));
                if (a == 1000 || a == 1002 || a == 1004 || a == 11 || a == 12) log = false;
            }
            // Game
            else if (raw.GetByte("c") == 1) {
                ISFSObject p = raw.GetSFSObject("p");
                if (p.GetUtfString("c") == "GameStart") {
                    ISFSObject p2 = p.GetSFSObject("p");
                    //NetworkMonster monster1 = p2.GetSFSArray("teamMonsters").GetClass(0) as NetworkMonster;
                    //monster1.luma = true;
                    //monster1.Nickname = "gay";
                    p2.PutUtfString("nck", "nitro.");
                    p2.PutInt("tid", 1337);
                }

                // Player movements
                int a = Convert.ToInt32(raw.GetShort("a"));
                //if (a == 1000 || a == 1002 || a == 1004 || a == 11) log = false;
                if (a == 13) {
                    string c = p.GetUtfString("c");
                    if (c == "UPP" || c.StartsWith("PA.")) log = false;
                }
            }
            if (log) Logger.LogLine($"Receiving message (ID {Convert.ToInt32(raw.GetShort("a"))}):\n{raw.GetDump()}", ConsoleColor.Cyan);
            return true;
        }

        private static void GLAwakePatch() {
            typeof(ObscuredInt).GetField("cryptoKey", BindingFlags.Static | BindingFlags.NonPublic).SetValue(null, 0);
            typeof(ObscuredByte).GetField("cryptoKey", BindingFlags.Static | BindingFlags.NonPublic).SetValue(null, (byte)0);
            typeof(ObscuredFloat).GetField("cryptoKey", BindingFlags.Static | BindingFlags.NonPublic).SetValue(null, 0);
            typeof(ObscuredBool).GetField("cryptoKey", BindingFlags.Static | BindingFlags.NonPublic).SetValue(null, (byte)0);
            typeof(ObscuredVector3).GetField("cryptoKey", BindingFlags.Static | BindingFlags.NonPublic).SetValue(null, 0);
            typeof(ObscuredQuaternion).GetField("cryptoKey", BindingFlags.Static | BindingFlags.NonPublic).SetValue(null, 0);
            Logger.LogLine("[PatchManager] Removed obscured types, have fun!", ConsoleColor.Cyan);
        }

    }
}
