namespace PartsList
{


int AddPart(CraftEditor@ editor, EditorFeature@ feature, VariantMap& args)
{
    Print("Add part?");
    return 0;
}

void SetupPartsList(UIElement@ panelPartsList)
{
    
    ListView@ partList = panelPartsList.GetChild("Content").GetChild("ListView");
    UIElement@ itemContainer = partList.GetChild("SV_ScrollPanel").GetChild("LV_ItemContainer");

    // Access the OSP hidden scene to get list of parts
    Array<Node@> categories = osp.hiddenScene.GetChild("Parts").GetChildren();

    for (uint i = 0; i < categories.length; i ++)
    {   
        Array<Node@> parts = categories[i].GetChildren();
        for (uint j = 0; j < parts.length; j ++)
        {
            // Make buttons for each part
            Print("Making button for: " + parts[j].name);
            Button@ butt = Button();
            butt.SetStyleAuto();
            butt.SetMinSize(64, 48);
            butt.SetMaxSize(64, 48);
            butt.vars["N"] = parts[j];
            
            BorderImage@ rocketPicture = BorderImage("Thumbnail");
            rocketPicture.SetSize(40, 40);
            rocketPicture.SetPosition(4, 4);
            rocketPicture.texture = cache.GetResource("Texture2D", "Textures/NoThumbnail.png");
            butt.AddChild(rocketPicture);
            
            Text@ someIndicator = Text("TypeIndicator");
            someIndicator.text = "42";
            someIndicator.SetFont(cache.GetResource("Font", "Fonts/BlueHighway.ttf"), 8);
            someIndicator.SetPosition(47, 4);
            butt.AddChild(someIndicator);
            
            itemContainer.SetSize(itemContainer.size.x, itemContainer.size.y + 70);
            itemContainer.AddChild(butt);
            SubscribeToEvent(butt, "Pressed", "PartsList::HandlePartButtonPressed");
        }
    }
    
}

void HandlePartButtonPressed(StringHash eventType, VariantMap& eventData)
{
    Node@ prototype = cast<Node@>(cast<UIElement@>(eventData["Element"].GetPtr()).vars["N"].GetPtr());
    Print("Part clicked");
    
    Print("Display Name : " + prototype.vars["name"].GetString());
    Print("Description  : " + prototype.vars["description"].GetString());
    Print("Manufacturer : " + prototype.vars["manufacturer"].GetString());
    Print("Country      : " + prototype.vars["country"].GetString());
    
    Node@ clone = prototype.Clone();
    clone.enabled = true;
   
    Array<Node@> models = prototype.GetChildrenWithComponent("StaticModel", true);
    
    for (uint i = 0; i < models.length; i ++) 
    {
        cast<StaticModel>(models[i].GetComponent("StaticModel")).material.scene = scene;
    }
    
    //grabbed = clone;
    //subject.AddChild(clone);
}

}
