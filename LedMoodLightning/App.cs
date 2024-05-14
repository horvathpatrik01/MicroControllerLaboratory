using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace LEDMoodlightning
{
    public class App            //statikus osztály amely egy listában tárolja az animációkat
    {
        private List<Animations> animationlist=new List<Animations>();
        private static App theApp;
        private MoodLED mainForm;
        private Animations activeanimation;
        private static string acknowledge;
        private static bool buttonenabled;
        
        public static App Instance
        {
            get { return theApp; }
           
        }

        //ha meg van nyitva az uart port akkor engedélyezzük                               
        public void SetLoadButtonEnabled()
        {
            if (mainForm.OpenPort.Enabled == true)
                buttonenabled = false;
            else
                buttonenabled = true;
            if(mainForm.TabControl.TabCount!=0)
                activeanimation.setButton();

        }
        public static bool ButtonEnabled { get { return buttonenabled; } }
        public static string Acknowledge
        {
            get { return acknowledge; }
            set { acknowledge = value; }
        }
        public static void Initialize(MoodLED form)
        {
            theApp = new App();
            theApp.mainForm = form;
            acknowledge = null;
            buttonenabled = false;
        }
        public MoodLED MainForm
        {
            get { return mainForm; }
        }

        public Animations GetActiveAnimation
        {
            get { return activeanimation; }
        }

        //aktív animáció bezárása és törlése a tabok közül valamint fogadja a hardver által küldött választ
        public void CloseActiveAnimation()
        {
            if (mainForm.TabControl.TabPages.Count == 0)
                return;
            UpdateActiveAnimation();
            SendAnimationSettings(true);
            RecieveAcknowledge();
            if (acknowledge != null)
            {
                if (String.Compare(acknowledge, "OK") == 0)
                {
                    foreach (Animations anim in animationlist)
                    {
                        if (anim.Animindex > mainForm.TabControl.SelectedIndex)
                        {
                            anim.Animindex--;
                        }
                    }


                    mainForm.TabControl.TabPages.RemoveByKey(activeanimation.Name);
                    animationlist.Remove(activeanimation);
                    mainForm.Animcount--;
                    Console.WriteLine("The selected animation has been deleted.");
                    MessageBox.Show( acknowledge, "The selected animation has been deleted.", MessageBoxButtons.OK,MessageBoxIcon.Information );
                }
                else if (String.Compare(acknowledge, "ERROR") == 0)
                {
                    Console.WriteLine("There are some problems with the bytes has been sent.");
                    MessageBox.Show(acknowledge, "There are some problems with the bytes has been sent.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                acknowledge = null;
            }
            else
            {
                Console.WriteLine("Nothing happened");
            }
        }


        //a hardver által küldött üzenet fogadása
        public void RecieveAcknowledge()
        {
            
            try
            {
                while (acknowledge == null && mainForm.SerialPort1.IsOpen)
                {

                    //Beérkezett adat kiolvasása
                    string rxData = mainForm.SerialPort1.ReadExisting().ToString();
                    //Beérkezett adat ellenőrzése
                    if (rxData != null)
                    {
                        acknowledge = rxData;
                        //kiírt adat frissítése
                        Console.WriteLine(acknowledge);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "No connection detected.", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        //új animáció hozzáadása a tabhoz, nézet csatolása az animációhoz
        public void NewAnimation()
        {
            NewAnimForm form = new NewAnimForm();
            if (form.ShowDialog()!=DialogResult.OK)
                return;
            if (animationlist.Count != 0)
            {
                foreach (Animations anim in animationlist)
                {
                    if (anim.Name == form.FontName)
                    {
                        form.Close();
                        return;
                    }
                }
            }
            Animations Anim=new Animations(form.FontName);
            AnimView animView = new AnimView();
            Anim.AttachView(animView);
            animView.AttachToAnim(Anim);
            animationlist.Add(Anim);

            mainForm.TabControl.TabPages.Add(form.FontName, form.FontName);
            //AnimView animView = new AnimView();
            TabPage tabPage = mainForm.TabControl.TabPages[form.FontName];
            tabPage.Controls.Add(animView);
            animView.Dock = DockStyle.Fill;

            mainForm.TabControl.SelectTab(tabPage);
            mainForm.TabControl.Visible = true;
            Anim.Animindex = (byte)mainForm.Animcount;
            UpdateActiveAnimation();
            mainForm.Animcount++;
        }

        //az éppen kiválasztott animáció beállítása
        public void UpdateActiveAnimation()
        {
            if(mainForm.TabControl.TabPages.Count == 0)
                activeanimation = null;
            else
            {
                foreach(Animations anim in animationlist)
                {
                    if(anim.Name==mainForm.TabControl.SelectedTab.Name)
                        activeanimation = anim;
                }
            }
        }

        // az animáció beállításainak elküldése
        public void SendAnimationSettings(bool isclosing)
        {
            UpdateActiveAnimation();
            UInt16 controlamount=0;
            byte[] controlamountbuffer = new byte[2] { 0, 0 };
            byte[] txBuffer= activeanimation.Animationsettings;
            txBuffer[15] = activeanimation.Animindex;
            for (int i = 0; i < txBuffer.Length; i++)
            {
                controlamount += txBuffer[i];
               // Console.WriteLine(txBuffer[i]); teszteléshez
            }
            if (isclosing)          //Ha bezárjuk az alkalmazást, akkor még a c értékével megnöveljük az ellenőrző összeget
            {
                controlamount += 'c';
            }
            //Console.WriteLine(controlamount);
            controlamountbuffer[0] = (byte)(controlamount >> 8);    //ellenőrző összeg felső 8 bit
            controlamountbuffer[1] = (byte)(controlamount & 0x00FF);//ellenőrző összeg alsó 8 bit
            try
            {
                if (!isclosing)
                {
                    if (mainForm.SerialPort1.IsOpen)
                    {
                        mainForm.SerialPort1.Write("@");//új animáció hozzáadásához tartozó üzenet küldése
                        mainForm.SerialPort1.Write(txBuffer, 0, 16);//beállítások küldése
                        
                    }
                    else
                        throw new ArgumentNullException(nameof(mainForm.SerialPort1));
                }
                else
                {
                    if (mainForm.SerialPort1.IsOpen)
                    {
                        mainForm.SerialPort1.Write("c");   //animáció törléséhez tartozó kód küldése
                        mainForm.SerialPort1.Write(txBuffer, 0, 16);
                    }
                    else
                        throw new ArgumentNullException(nameof(mainForm.SerialPort1));
                }
                mainForm.SerialPort1.Write(controlamountbuffer, 0, 2);//ellenőrző összeg küldése
            }
            catch (ArgumentNullException except) { Console.WriteLine("Serial Port is not open:" + except.ParamName.ToString()); }
        }
    }
}