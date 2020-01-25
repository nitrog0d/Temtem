using System.Linq;
using System.Reflection;
using Temtem.Core;
using Temtem.World;

namespace TemtemCheat.Cheat {
    class InstanceManager {

        // https://nitro.moe/Go9sl.png
        public static MethodInfo battleMethod = InstanceManager.battleMethod = typeof(WildMonstersLogic).GetMethods(BindingFlags.Public | BindingFlags.Instance).Where(x =>
            (x.ReturnType == typeof(void)) &&
            (x.GetParameters().Length == 1) &&
            (x.GetParameters()[0].ParameterType == typeof(SpawnZoneDefinition))
            ).ToArray()[12];

    }
}
