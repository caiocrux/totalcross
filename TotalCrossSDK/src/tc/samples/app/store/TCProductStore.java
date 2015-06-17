package tc.samples.app.store;

import totalcross.sys.*;
import totalcross.ui.*;

public class TCProductStore extends MainWindow
{

   public TCProductStore()
   {
      setUIStyle(Settings.Holo);
      setBackColor(0x006000);
      Settings.uiAdjustmentsBasedOnFontHeight = true;
   }
   
   public void initUI()
   {
      new PSlogin().swapToTopmostWindow();
   }

}
