<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.8" tiledversion="1.8.2" name="meta_tiles" tilewidth="32" tileheight="32" spacing="1" margin="1" tilecount="2" columns="2">
 <properties>
  <property name="Collidable" type="bool" value="true"/>
 </properties>
 <image source="meta_tiles.png" width="67" height="34"/>
 <tile id="0">
  <properties>
   <property name="Collidable" type="bool" value="true"/>
   <property name="collisionType" value="Collide"/>
  </properties>
 </tile>
 <tile id="1">
  <properties>
   <property name="collisionType" value="Collect"/>
  </properties>
 </tile>
</tileset>