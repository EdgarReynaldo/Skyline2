



#ifndef SettingsWidget_HPP
#define SettingsWidget_HPP




extern const unsigned int TOPIC_SETTING_WIDGET;

enum SETTING_WIDGET_MSGS {
   DIFFICULTY_SELECTION_CHANGED = 0,
   CUSTOM_VALUE_CHANGED =1
};

class SettingWidget : public WidgetBase {
private :
   TextWidget setting_text;
   GenericRadioButton difficulty_radio;
   TextWidget value_text;
   TextInput input_box;
   
   int setting_text_width;
   int value_text_width;
   int padding;
   
   int ivars[NUM_DIFFICULTIES];
   float fvars[NUM_DIFFICULTIES];
   
   int ival;
   float fval;
   TEXT_INPUT_TYPE input_type;
   DIFFICULTY difficulty;
   

   void ResetPositions();
   int MaxHeight();


public :
///   RadioButtonItem(std::string name , BUTTON_CLASS btnclass , BUTTON_TYPE btype , FONT* textfont , std::string text_label ,
///                   int btnw , int btnh , int padding , Rectangle position , UINT wflags = DEFAULT_FLAGS);
///   RadioButton(std::string name , RADIO_TYPE rtype , Rectangle position , int padding = 0 , UINT wflags = DEFAULT_FLAGS);
///   TextWidget(std::string name , std::string text_str , int tx , int ty , UINT wflags = DEFAULT_FLAGS);
///   TextWidget(std::string name , int text_val , int tx , int ty , UINT wflags = DEFAULT_FLAGS);
///   TextWidget(std::string name , double text_val , int tx , int ty , UINT wflags = DEFAULT_FLAGS);
///   TextWidget(std::string name , double text_val , unsigned int min_width , unsigned int num_decimals ,
///              int tx , int ty , UINT wflags = DEFAULT_FLAGS);
///   TextInput(std::string name , TEXT_INPUT_TYPE type , FONT* textfont , int num_chars_wide , int max_num_chars ,
///               int xpos , int ypos , UINT wflags = DEFAULT_FLAGS);
   SettingWidget(const char* description , int pad);

   virtual WidgetMsg Update(double tsec);
   virtual WidgetMsg CheckInputs(int msx , int msy);
   virtual void DisplayOn(BITMAP* bmp , int x , int y);
   virtual void QueueUserMessage(const WidgetMsg& wmsg);

   virtual void SetWidgetColors(const WidgetColorset& colors);
   virtual void UseWidgetColorset(WidgetColorset* colorset);
   virtual void UseIndividualColorset();

   virtual void SetDrawPos(int xpos , int ypos);
   virtual void SetDrawDimensions(int width , int height);// does nothing
   virtual void SetArea(int xpos , int ypos , int width , int height);// only sets position
   virtual void SetArea(const Rectangle& r);// only sets position
  
   virtual void SetHoverState(bool state);
   virtual void SetFocusState(bool state);

   

   void SetColumnWidths(int setting_text_box_width , int value_text_box_width);
   void SetupRadioButtonItems(BITMAP* btn_up , BITMAP* btn_down , BITMAP* btn_hover);
   void SetValues(DIFFICULTY d , int easy , int medium , int hard , int custom , int min , int max);
   void SetValues(DIFFICULTY d , float easy , float medium , float hard , float custom , float min , float max);
   void SetDifficulty(DIFFICULTY d);
   void SetCustomValue(int value);
   void SetCustomValue(float value);
   
   int SettingTextWidth() {return setting_text_width;}
   int ValueTextWidth() {return value_text_width;}
   DIFFICULTY SelectedDifficulty() {return (DIFFICULTY)difficulty_radio.ActiveItemIndex();}
   int SettingTextX() {return area.X();}
   Rectangle RadioButtonArea(DIFFICULTY d);
   int RadioButtonX() {return area.X() + difficulty_radio.Area().X();}
   int GetCustomIntegerValue();
   float GetCustomFloatValue();

   virtual void PrintTo(std::ostream& os , Indenter& indent) const;

   friend std::ostream& operator<<(std::ostream& os , const SettingWidget& sw);

};




#endif // SettingsWidget_HPP
