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
using System.Collections.Generic;
using Temtem.UI;
using Temtem.Inventory;
using Temtem.Players;

namespace TemtemCheat.Cheat {
    class GUIManager : MonoBehaviour {

        private bool guiEnabled = false;
        private Rect guiRect = new Rect(20, 20, 200, 200);

        private void Awake() {
            Logger.LogLine("[GUIManager] GUI Manager started, press F1 to open it.");
        }

        private void Update() {
            if (Input.GetKeyDown(KeyCode.F1)) {
                guiEnabled = !guiEnabled;
            }
        }

        private void OnGUI() {
            if (guiEnabled) {
                guiRect = GUI.Window(0, guiRect, CheatWindow, "Temtem Cheat");
            }
        }

        private PropertyInfo smartFoxProperty = typeof(NetworkLogic).GetProperties(BindingFlags.Public | BindingFlags.Static).Where(x => x.PropertyType == typeof(SmartFox)).ToArray()[0];
        private PropertyInfo uiManagerProperty = typeof(UIManager).GetProperties(BindingFlags.Public | BindingFlags.Static).Where(x => x.PropertyType == typeof(UIManager)).ToArray()[0];

        public enum Menu {
            None,
            Unknown1,
            Backpack,
            ShopBuy,
            ShopSell,
            Squad,
            Customize,
            TemtemStats,
            Tempedia,
            StorageBox,
            Trade,
            Dialogue,
            Unknown12,
            Unknown13,
            Unknown14,
            Unknown15,
            Unknown16,
            TamerInfo,
            Emote,
            Breed,
            Map,
            Unknown21,
            Notifications,
            TamerSearch
        }

        private void CheatWindow(int windowID) {
            CheatOptions.instantAggro = GUI.Toggle(new Rect(15, 25, 170, 20), CheatOptions.instantAggro, "Instant Aggro");
            CheatOptions.noAggro = GUI.Toggle(new Rect(15, 50, 170, 20), CheatOptions.noAggro, "No Aggro");
            if (GUI.Button(new Rect(15, 75, 170, 20), "Heal Temtems")) {
                SmartFox smartFoxInstance = smartFoxProperty.GetValue(null) as SmartFox;
                if (smartFoxInstance != null) {
                    Logger.LogLine("[GUIManager] Sending Heal Packet", ConsoleColor.Green);
                    smartFoxInstance.Send(new ExtensionRequest("gameplay.HealTeam", new SFSObject()));
                }
            }
            if (GUI.Button(new Rect(15, 100, 170, 20), "Open Storage Box")) {
                UIManager uiManagerInstance = uiManagerProperty.GetValue(null) as UIManager;
                if (uiManagerInstance != null) {
                    PropertyInfo menu = typeof(UIManager).GetProperties(BindingFlags.Public | BindingFlags.Instance).Where(x =>
                    (x.PropertyType.IsEnum) &&
                    (x.PropertyType.GetEnumValues().Length == 24)
                    ).ToArray()[0];
                    menu.SetValue(uiManagerInstance, (int)Menu.StorageBox);
                }
            }
            if (GUI.Button(new Rect(15, 125, 170, 20), "Add all Items to Shop")) {
                AllItemDefinitions instance = Resources.FindObjectsOfTypeAll<AllItemDefinitions>()[0];
                List<ItemDefinition> list = new List<ItemDefinition>();

                // Thank you so much EquiFox!
                foreach (ItemDefinition item in instance.GeneralItemsList.Union(instance.BundleItemsList)
                    .Union(instance.CaptureItemsList)
                    .Union(instance.CosmeticItemsList)
                    .Union(instance.CourseItemsList)
                    .Union(instance.EmoteItemsList)
                    .Union(instance.HardcodedPromoCodeItemsList)
                    .Union(instance.HeldItemsList)
                    .Union(instance.KeyItemsList)
                    .Union(instance.MedicineItemsList)
                    .Union(instance.StickerItemsList)
                    .Union(instance.TintBundleItemsList)
                    .Union(instance.TintItemsList)) list.Add(item);

                BuyableItemList[] shops = Resources.FindObjectsOfTypeAll<BuyableItemList>();
                Console.WriteLine(shops.Length);
                List<int> ids = new List<int>();
                foreach (var item in list) {
                    if (item != null && item.Price != 0) ids.Add(item.Id);
                }
                foreach (var shop in shops) shop.ShopItemIds = ids;
            }
            GUI.Label(new Rect(40, 175, 170, 20), "Made by nitro.#0007");
            GUI.DragWindow();
        }

    }
}
