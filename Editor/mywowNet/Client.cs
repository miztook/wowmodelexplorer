using System;
using System.Runtime.InteropServices;

namespace mywowNet
{
    public partial class Client : IDisposable
    {
       private static Client instance = null;

        public Client()
        {
            if (instance == null)
            {
                client_create();

                _player = new Player(Client_getPlayer());
                _world = new World(Client_getWorld());

                instance = this;
            }
        }

        ~Client()
        {
            Dispose();
        }

        public static Client Instance
        {
            get { return instance; }
        }

        bool _disposed = false;
        public void Dispose()
        {
            if (!_disposed)
            {
                client_destroy();
                _disposed = true;
            }
        }

        Player _player;
        World _world;

        public Player GetPlayer() 
        {
            return _player;
        }

        public World GetWorld()
        {
            return _world;
        }

        public void Tick(uint delta)
        {
            Client_tick(delta);
        }

        public void OnMouseWheel(float fDelta)
        {
            Client_onMouseWheel(fDelta);
        }

        public void OnMouseMove(float pitchDegree, float yawDegree)
        {
            Client_onMouseMove(pitchDegree, yawDegree);
        }
    }
}