


#include "SettingsWidget.hpp"


#include <iostream>
using std::endl;

const unsigned int TOPIC_SETTING_WIDGET = NextFreeTopicId();


void SettingWidget::ResetPositions() {
   int maxheight = MaxHeight();
   setting_text.SetTextPosEx(setting_text_width - 1, maxheight/2 , TEXT_RIGHT , TEXT_VCENTER);
   int x = setting_text_width + padding;
   difficulty_radio.SetDrawPos(x , (maxheight - difficulty_radio.Area().H())/2);
   x += difficulty_radio.Area().W() + padding;
   value_text.SetTextPosEx(x + value_text_width - 1, maxheight/2 , TEXT_RIGHT , TEXT_VCENTER);
   x += value_text_width + padding;
   input_box.SetDrawPos(x , (maxheight - input_box.Area().H())/2);
   WidgetBase::SetDrawDimensions(setting_text_width + padding + difficulty_radio.Area().W() + padding +
                                 value_text_width + padding + input_box.Area().W() , maxheight);
}



int SettingWidget::MaxHeight() {
   int maxh = setting_text.Area().H();
   int h = difficulty_radio.Area().H();
   if (h > maxh) {maxh = h;}
   h = value_text.Area().H();
   if (h > maxh) {maxh = h;}
   h = input_box.Area().H();
   if (h > maxh) {maxh = h;}
   return maxh;
}



SettingWidget::SettingWidget(const char* description , int pad) :
      WidgetBase(string("SettingWidget : ") + description , Rectangle(0,0,1,1)),
      setting_text(string("description text - ") + description , description , 0 , 0),
      difficulty_radio(string(description) + " - radio" , RADIO_HORIZONTAL , Rectangle(0,0,1,1) , pad),
      value_text(string("value text - ") + description , "" , 0 , 0),
      input_box(string("input box - ") + description , INTEGER_INPUT , font , 7 , 7 , 0 , 0),
      setting_text_width(0),
      value_text_width(0),
      padding(pad),
      ivars(),
      fvars(),
      ival(0),
      fval(0.0f),
      input_type(TEXT_INPUT),
      difficulty(EASY)
{
   setting_text_width = setting_text.Area().W();
   setting_text.SetParent(this);
   difficulty_radio.SetParent(this);
   value_text.SetParent(this);
   input_box.SetParent(this);
   setting_text.UseWidgetColorset(wcols);
   difficulty_radio.UseWidgetColorset(wcols);
   value_text.UseWidgetColorset(wcols);
   input_box.UseWidgetColorset(wcols);
}



WidgetMsg SettingWidget::Update(double tsec) {
   return input_box.Update(tsec);
}



WidgetMsg SettingWidget::CheckInputs(int msx , int msy) {
   int lmsx = msx - area.X();
   int lmsy = msy - area.Y();
   if (flags & HOVER) {
      difficulty_radio.SetHoverState(difficulty_radio.Area().Contains(lmsx , lmsy));
   }
   int msg = difficulty_radio.CheckInputs(lmsx , lmsy).Messages();
   msg |= input_box.CheckInputs(lmsx , lmsy).Messages();
   return WidgetMsg(this , TOPIC_DIALOG , msg);
}



void SettingWidget::DisplayOn(BITMAP* bmp , int x , int y) {
   int lx = area.X();
   int ly = area.Y();
   setting_text.DisplayOn(bmp , x + lx , y + ly);
   difficulty_radio.DisplayOn(bmp , x + lx , y + ly);
   value_text.DisplayOn(bmp , x + lx , y + ly);
   input_box.DisplayOn(bmp , x + lx , y + ly);
   ClearRedrawFlag();
}



void SettingWidget::QueueUserMessage(const WidgetMsg& wmsg) {
   const WidgetMsg radio_click(&difficulty_radio , TOPIC_RADIO_BUTTON , RADIO_BTN_CLICKED);
   const WidgetMsg input_change(&input_box , TOPIC_TEXT_INPUT , TEXT_VALUE_ENTERED);
   WidgetMsg qmsg = wmsg;
   if (wmsg == radio_click) {
      difficulty = (DIFFICULTY)difficulty_radio.ActiveItemIndex();
      switch (input_type) {
         case INTEGER_INPUT :
            ival = ivars[difficulty];
            input_box.SetIntegerValue(ival);
            value_text.SetText(ival);
            break;
         case FLOATING_POINT_INPUT :
            fval = fvars[difficulty];
            input_box.SetFloatValue(fval);
            value_text.SetText(fval , 8 , 2);
            break;
         default :
            throw EagleError("Setting widget input type is wrong wrong wrong...\n");
      }
      if (difficulty == CUSTOM) {
         input_box.ShowWidget();
      }
      else {
         input_box.HideWidget();
      }
//      SetBgRedrawFlag();
      qmsg = WidgetMsg(this , TOPIC_SETTING_WIDGET , DIFFICULTY_SELECTION_CHANGED);
   }
   else if (wmsg == input_change) {
      switch (input_type) {
         case INTEGER_INPUT :
            ivars[CUSTOM] = input_box.GetIntValue();
            ival = ivars[CUSTOM];
            value_text.SetText(ival);
            break;
         case FLOATING_POINT_INPUT :
            fvars[CUSTOM] = input_box.GetFloatValue();
            fval = fvars[CUSTOM];
            value_text.SetText(fval , 8 , 2);
            break;
         default :
            throw EagleError("Setting widget input type is wrong wrong wrong...\n");
      }
//      SetBgRedrawFlag();
      qmsg = WidgetMsg(this , TOPIC_SETTING_WIDGET , CUSTOM_VALUE_CHANGED);
   }
   WidgetBase::QueueUserMessage(qmsg);
}



void SettingWidget::SetWidgetColors(const WidgetColorset& colors) {
   WidgetBase::SetWidgetColors(colors);
   setting_text.UseWidgetColorset(wcols);
   difficulty_radio.UseWidgetColorset(wcols);
   value_text.UseWidgetColorset(wcols);
   input_box.UseWidgetColorset(wcols);
}



void SettingWidget::UseWidgetColorset(WidgetColorset* colorset) {
   WidgetBase::UseWidgetColorset(colorset);
   setting_text.UseWidgetColorset(colorset);
   difficulty_radio.UseWidgetColorset(colorset);
   value_text.UseWidgetColorset(colorset);
   input_box.UseWidgetColorset(colorset);
}



void SettingWidget::UseIndividualColorset() {
   WidgetBase::UseIndividualColorset();
   setting_text.UseWidgetColorset(wcols);
   difficulty_radio.UseWidgetColorset(wcols);
   value_text.UseWidgetColorset(wcols);
   input_box.UseWidgetColorset(wcols);
}



void SettingWidget::SetDrawPos(int xpos , int ypos) {
   WidgetBase::SetDrawPos(xpos,ypos);
}



void SettingWidget::SetDrawDimensions(int width , int height) {
   // does nothing
}



void SettingWidget::SetArea(int xpos , int ypos , int width , int height) {
   SetDrawPos(xpos,ypos);
}



void SettingWidget::SetArea(const Rectangle& r) {
   SetDrawPos(r.X() , r.Y());
}



void SettingWidget::SetHoverState(bool state) {
   WidgetBase::SetHoverState(state);
   if (!state) {
      difficulty_radio.SetHoverState(false);
   }
   SetRedrawFlag();
}



void SettingWidget::SetFocusState(bool state) {
   WidgetBase::SetFocusState(state);
   input_box.SetFocusState(state);
}



void SettingWidget::SetColumnWidths(int setting_text_box_width , int value_text_box_width) {
   int stw = setting_text_box_width;
   int vtw = value_text_box_width;
   if (stw < setting_text.Area().W()) {stw = setting_text.Area().W();}
   if (vtw < value_text.Area().W()) {stw = value_text.Area().W();}
   setting_text_width = stw;
   value_text_width = vtw;
   ResetPositions();
}



void SettingWidget::SetupRadioButtonItems(BITMAP* btn_up , BITMAP* btn_down , BITMAP* btn_hover) {
//   bool LoadImages(const char* up_pic_file , const char* down_pic_file , const char* hover_pic_file , const char* disabled_pic_file ,
//                     BMP_MEM_TYPE bmp_mem_type , BMP_DRAW_TYPE bmp_draw_type);
//   IconRadioButtonItem(std::string text_label , std::string name);
   ASSERT(btn_up);
   ASSERT(btn_down);
   difficulty_radio.ClearItems();
   difficulty_radio.SetDrawDimensions(NUM_DIFFICULTIES*btn_up->w + (NUM_DIFFICULTIES - 1)*padding , btn_up->h);
   for (int i = 0 ; i < NUM_DIFFICULTIES ; ++i) {
//IconButton::IconButton(std::string name , ICON_DISPLAY_TYPE dtype , Rectangle position , UINT wflags) :
      IconButton* iconbtn = new IconButton(setting_text.Text() + "_iconbtn_" + DiffToStr(DIFFICULTY(i)) ,
                                           ICON_DISPLAY_CENTER , Rectangle(0 , 0 , btn_up->w , btn_up->h) , DEFAULT_FLAGS);
      iconbtn->SetImages(btn_up , btn_down , btn_hover , 0 , MEMORY , TRANSPARENT);
      difficulty_radio.AddItem(iconbtn , true);
   }
   difficulty_radio.SetActiveItem(UINT(difficulty));
   ResetPositions();
}



void SettingWidget::SetValues(DIFFICULTY d , int easy , int medium , int hard , int custom , int min , int max) {
   input_type = INTEGER_INPUT;

   if (max    < min) {max    = min;}

   if (easy   < min) {easy   = min;}
   if (medium < min) {medium = min;}
   if (hard   < min) {hard   = min;}
   if (custom < min) {custom = min;}
   if (easy   > max) {easy   = max;}
   if (medium > max) {medium = max;}
   if (hard   > max) {hard   = max;}
   if (custom > max) {custom = max;}

   difficulty = d;

   ivars[EASY] = easy;
   ivars[MEDIUM] = medium;
   ivars[HARD] = hard;
   ivars[CUSTOM] = custom;
   ival = ivars[difficulty];
   input_box.SetIntegerRange(min , ival , max);
   value_text.SetText(input_box.GetIntValue());
   value_text_width = value_text.Area().W();

   difficulty_radio.SetActiveItem(UINT(difficulty));
   
   if (difficulty == CUSTOM) {
      input_box.ShowWidget();
   }
   else {
      input_box.HideWidget();
   }
}



void SettingWidget::SetValues(DIFFICULTY d , float easy , float medium , float hard , float custom , float min , float max) {
   input_type = FLOATING_POINT_INPUT;

   if (max    < min) {max    = min;}

   if (easy   < min) {easy   = min;}
   if (medium < min) {medium = min;}
   if (hard   < min) {hard   = min;}
   if (custom < min) {custom = min;}
   if (easy   > max) {easy   = max;}
   if (medium > max) {medium = max;}
   if (hard   > max) {hard   = max;}
   if (custom > max) {custom = max;}

   difficulty = d;

   fvars[EASY] = easy;
   fvars[MEDIUM] = medium;
   fvars[HARD] = hard;
   fvars[CUSTOM] = custom;
   fval = fvars[difficulty];
   input_box.SetFloatRange(min , fval , max);
   value_text.SetText(input_box.GetFloatValue() , 8 , 2);
   value_text_width = value_text.Area().W();
   
   difficulty_radio.SetActiveItem(UINT(difficulty));
   
   if (difficulty == CUSTOM) {
      input_box.ShowWidget();
   }
   else {
      input_box.HideWidget();
   }
}



void SettingWidget::SetDifficulty(DIFFICULTY d) {
   difficulty = d;
   difficulty_radio.SetActiveItem((UINT)d);
   switch (input_type) {
      case INTEGER_INPUT :
         ival = ivars[d];
         input_box.SetIntegerValue(ival);
         value_text.SetText(input_box.GetIntValue());
         ivars[d] = ival = input_box.GetIntValue();
         break;
      case FLOATING_POINT_INPUT :
         fval = fvars[d];
         input_box.SetFloatValue(fval);
         value_text.SetText(input_box.GetFloatValue() , 8 , 2);
         fvars[d] = fval = input_box.GetFloatValue();
         break;
      case TEXT_INPUT :
         throw EagleError("Huh? Why is the SettingWidget's input_type equal to TEXT_INPUT?\n");
         break;
   }
   if (difficulty == CUSTOM) {
      input_box.ShowWidget();
   }
   else {
      input_box.HideWidget();
   }
}



void SettingWidget::SetCustomValue(int value) {
   ASSERT(input_type == INTEGER_INPUT);
   if (value < input_box.GetIntMin()) {value = input_box.GetIntMin();}
   if (value > input_box.GetIntMax()) {value = input_box.GetIntMax();}
   ivars[CUSTOM] = value;
   if (difficulty == CUSTOM) {
      ival = value;
      input_box.SetIntegerValue(ival);
      value_text.SetText(ival);
   }
}



void SettingWidget::SetCustomValue(float value) {
   ASSERT(input_type == FLOATING_POINT_INPUT);
   if (value < input_box.GetFloatMin()) {value = input_box.GetFloatMin();}
   if (value > input_box.GetFloatMax()) {value = input_box.GetFloatMax();}
   fvars[CUSTOM] = value;
   if (difficulty == CUSTOM) {
      fval = value;
      input_box.SetFloatValue(fval);
      value_text.SetText(fval , 8 , 2);
   }
}



Rectangle SettingWidget::RadioButtonArea(DIFFICULTY d) {
   Rectangle r = difficulty_radio.ItemIndexToButton(d)->Area();
   r.MoveBy(area.X() , area.Y());
   return r;
}



int SettingWidget::GetCustomIntegerValue() {
   ASSERT(input_type == INTEGER_INPUT);
   return ivars[CUSTOM];
}



float SettingWidget::GetCustomFloatValue() {
   ASSERT(input_type == FLOATING_POINT_INPUT);
   return fvars[CUSTOM];
}



void SettingWidget::PrintTo(std::ostream& os , Indenter& indent) const {
   os << indent << "Setting widget {" << endl;
   ++indent;
   PrintBaseTo(os , indent);
   os << indent << "setting_text_width = " << setting_text_width << " , value_text_width = " << value_text_width << " , padding = " << padding << endl;
   os << indent << "input_type = " << ToString(input_type) << " , difficulty = " << DiffToStr(difficulty) << endl;
   os << indent << "ivars = " << StringPrintF("%i,%i,%i,%i" , ivars[EASY] , ivars[MEDIUM] , ivars[HARD] , ivars[CUSTOM]) << endl;
   os << indent << "fvars = " << StringPrintF("%f,%f,%f,%f" , fvars[EASY] , fvars[MEDIUM] , fvars[HARD] , fvars[CUSTOM]) << endl;
   setting_text.PrintTo(os , indent);
   difficulty_radio.PrintTo(os , indent);
   value_text.PrintTo(os , indent);
   input_box.PrintTo(os , indent);
   --indent;
   os << indent << "}" << endl;
}



std::ostream& operator<<(std::ostream& os , const SettingWidget& sw) {
   Indenter i(0,3);
   sw.PrintTo(os , i);
   return os;
}


