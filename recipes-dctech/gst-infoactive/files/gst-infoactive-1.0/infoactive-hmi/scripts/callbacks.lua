local y_ptr
local x_ptr
local MAX_TICKS = 6 
local SLIDER_HEIGHT = 22
local TRACK_WIDTH = 28
local init_flag=0

function init(mapargs)
  if init_flag == 0 then
    init_flag=1
  end
end

function set_equalizer(eq_num, num)
  local data = {}
  local name_str = "EQLayer.table%d.alpha.%d.1"
  
  for i=1, 6 do
    local name = string.format(name_str, eq_num, i)
    if i >  (6 - num) then
      data[name] = 255
    else
      data[name] = 0
    end
  end
  gre.set_data(data)
end

function cb_slider_moving(mapargs) 
  local slider_num = string.sub(mapargs.context_control,-1)
  local band = gre.get_value(mapargs.context_control..".band")
  local cur_offset = gre.get_value(mapargs.context_control..".y_offset")
  local data={}
  if gre.get_value(mapargs.context_control..".sliderPressed") == 1 then
    local data=gre.get_layer_attrs("EQLayer","y")
    local y_pos=mapargs.context_event_data.y-gre.get_value(mapargs.context_control..".grd_y")-SLIDER_HEIGHT/2
    local slider_offset=y_pos-data["y"]
    local slider_ctrl_height=gre.get_value(mapargs.context_control..".grd_height")
    if slider_offset >= 0 and slider_offset < (slider_ctrl_height-SLIDER_HEIGHT) and slider_offset ~= cur_offset then
      gre.set_value(mapargs.context_control..".y_offset",slider_offset)
      local num_ticks = (MAX_TICKS + 1) - math.ceil(((slider_offset + SLIDER_HEIGHT/2) / (slider_ctrl_height-SLIDER_HEIGHT)) * MAX_TICKS)
      set_equalizer(slider_num, num_ticks)
      data[band]=1-slider_offset/(slider_ctrl_height-SLIDER_HEIGHT-1)
      gre.send_event_data("av_eq_ctrl","4f1 "..band,data,"media_player")
    end
  end
end

function cb_vol_moving(mapargs) 
  local current_offset = gre.get_value(mapargs.context_control..".x_offset")
  if gre.get_value(mapargs.context_control..".sliderPressed") == 1 then
    local slider_offset=mapargs.context_event_data.x - gre.get_value(mapargs.context_control..".grd_x")-SLIDER_HEIGHT/2
    local slider_ctrl_height=gre.get_value(mapargs.context_control..".grd_width")
    if slider_offset >= 0 and slider_offset < (slider_ctrl_height-SLIDER_HEIGHT) and current_offset ~= slider_offset then
      gre.set_value(mapargs.context_control..".x_offset",slider_offset)
      local data={}
      data["volumen"]=slider_offset/(slider_ctrl_height-SLIDER_HEIGHT-1)
      gre.send_event_data("av_vol_ctrl","4f1 volumen",data,"media_player")
    end
  end
end

function cb_mv_layer(mapargs) 
  if gre.get_value("EQLayer.eq_moving_ctrl.eqBasePressed")==1 then
    local lx=gre.get_layer_attrs("EQLayer","x")
    local xrpos=lx["x"] + mapargs.context_event_data.x - x_ptr
    x_ptr=mapargs.context_event_data.x
    local ly=gre.get_layer_attrs("EQLayer","y")
    local yrpos=ly["y"] + mapargs.context_event_data.y - y_ptr
    y_ptr=mapargs.context_event_data.y
    local data={}
    data["x"]=xrpos
    data["y"]=yrpos
    gre.set_layer_attrs("EQLayer",data)
  end
end

function cb_press_layer(mapargs) 
  x_ptr=mapargs.context_event_data.x
  y_ptr=mapargs.context_event_data.y
end

function cb_press_eq(mapargs) 
  local str = gre.get_value("LowerAVLayer.eq_btn.eq_btn_image")
  if string.find(str,"on") then
    str = string.gsub(str, "on", "off")
    gre.send_event("EQCloseStart")
  else
    str = string.gsub(str, "off", "on")
    gre.send_event("EQOpenStart")
  end
  gre.set_value("LowerAVLayer.eq_btn.eq_btn_image",str)
end

function cb_player_btn_pressed(mapargs)
  local str = mapargs.context_control
  local format = "1s0 ctrl"
  local ctrl_name = string.gsub(mapargs.context_control,mapargs.context_layer..".","")
  local image_var = gre.get_value(str.."."..ctrl_name.."_image")  
  local data={}
  if string.find(str,"play") then
    if string.find(image_var,"on") then
      image_var = string.gsub(image_var, "on", "off")
    else
      image_var = string.gsub(image_var, "off", "on")
    end
  elseif string.find(str,"stop") then
  elseif string.find(str,"pause") then
    if string.find(image_var,"on") then
      image_var = string.gsub(image_var, "on", "off")
    else
      image_var = string.gsub(image_var, "off", "on")
    end
  elseif string.find(str,"backward") then
  elseif string.find(str,"forward") then
  elseif string.find(str,"mute") then
    if string.find(image_var,"on") then
      image_var = string.gsub(image_var, "on", "off")
    else
      image_var = string.gsub(image_var, "off", "on")
    end
  elseif string.find(str,"repeat") then
    if string.find(image_var,"on") then
      image_var = string.gsub(image_var, "on", "off")
    else
      image_var = string.gsub(image_var, "off", "on")
    end
  elseif string.find(str,"pwr") then
    if string.find(image_var,"on") then
      image_var = string.gsub(image_var, "on", "off")
      data["state"]= "off"
    else
      image_var = string.gsub(image_var, "off", "on")
      data["state"]= "on"
    end
    format="1s0 ctrl 1s0 state"    
  end
  gre.set_value(str.."."..ctrl_name.."_image",image_var)
  data["ctrl"]=string.gsub(ctrl_name, "_btn", "")
  gre.send_event_data("av_player_ctrl",format,data,"media_player")
end


function cb_trackb_moving(mapargs) 
  local cur_offset = gre.get_value(mapargs.context_control..".x_offset")
  if gre.get_value(mapargs.context_control..".trackPressed") == 1 then
    local track_width=gre.get_value(mapargs.context_control..".grd_width")
    local x_offset=mapargs.context_event_data.x-gre.get_value(mapargs.context_control..".grd_x")-track_width+TRACK_WIDTH/2
    --local x_offset=mapargs.context_event_data.x-gre.get_value(mapargs.context_control..".grd_x")+TRACK_WIDTH/2
    if x_offset > (TRACK_WIDTH-track_width) and x_offset <= 0 and x_offset ~= cur_offset then
      gre.set_value(mapargs.context_control..".x_offset",x_offset)
      local data={}
      data["progress"]=1+(x_offset)/(track_width - TRACK_WIDTH-1)
      gre.send_event_data("av_progress_ctrl","4f1 progress",data,"media_player")
    end
  end
end


function cb_io_test(mapargs) 
-- Your code goes here ...
  local track_width=gre.get_value("LowerAVLayer.trackbar.grd_width")
  local x_offset
  --print("Received position:",mapargs.context_event_data.pos)
  --print("Received length:",mapargs.context_event_data.len)
  --print("track x:",gre.get_value("LowerAVLayer.trackbar.x_offset"))
  x_offset = (mapargs.context_event_data.pos - 1) * (track_width - TRACK_WIDTH-1) 
  --print("x_offset:",x_offset)
  gre.set_value("LowerAVLayer.trackbar.x_offset",x_offset)
end
