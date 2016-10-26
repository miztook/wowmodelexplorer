using System;
using System.Windows;
using System.Windows.Threading;

namespace WowModelExplorer.Controls.Splash
{
    public class MessageListener : DependencyObject
    {
        private static MessageListener mInstance;

        private MessageListener()
        {

        }

        public static MessageListener Instance
        {
            get
            {
                if (mInstance == null)
                    mInstance = new MessageListener();
                return mInstance;
            }
        }

        public void StartWork(string message, DispatcherOperationCallback worker)
        {
            Message = message;
            DispatcherHelper.DoEvents(worker);
        }

        public string Message
        {
            get { return (string)GetValue(MessageProperty); }
            set { SetValue(MessageProperty, value); }
        }

        public static readonly DependencyProperty MessageProperty =
           DependencyProperty.Register("Message", typeof(string), typeof(MessageListener), new UIPropertyMetadata(null));

    }
}
