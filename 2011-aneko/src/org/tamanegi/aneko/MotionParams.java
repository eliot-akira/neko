package org.tamanegi.aneko;

import java.io.IOException;
import java.util.HashMap;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import android.content.Context;
import android.content.res.Resources;
import android.util.AttributeSet;
import android.util.Xml;

public class MotionParams
{
    public enum MoveDirection
    {
        UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
    }

    public enum WallDirection
    {
        UP, DOWN, LEFT, RIGHT
    }

    private static final String TAG_MOTION_PARAMS = "motion-params";
    private static final String TAG_MOTION = "motion";
    private static final String TAG_ITEM = "item";
    private static final String TAG_REPEAT_ITEM = "repeat-item";

    private static final String ATTR_ACCELERATION = "acceleration";
    private static final String ATTR_MAX_VELOCITY = "maxVelocity";
    private static final String ATTR_DEACCELERATION = "deaccelerationDistance";
    private static final String ATTR_PROXIMITY = "proximityDistance";

    private static final String ATTR_INITIAL_STATE = "initialState";
    private static final String ATTR_AWAKE_STATE = "awakeState";
    private static final String ATTR_MOVE_STATE_PREFIX = "moveStatePrefix";
    private static final String ATTR_WALL_STATE_PREFIX = "wallStatePrefix";

    private static final String ATTR_STATE = "state";
    private static final String ATTR_DURATION = "duration";
    private static final String ATTR_NEXT_STATE = "nextState";
    private static final String ATTR_CHECK_WALL = "checkWall";
    private static final String ATTR_CHECK_MOVE = "checkMove";

    private static final String ATTR_ITEM_DRAWABLE = "drawable";
    private static final String ATTR_ITEM_DURATION = "duration";
    private static final String ATTR_ITEM_REPEAT_COUNT = "repeatCount";

    private static final int DEF_ACCELERATION = 160; // dp per sec^2
    private static final int DEF_MAX_VELOCITY = 100; // dp per sec
    private static final int DEF_DEACCELERATE_DISTANCE = 100; // dp
    private static final int DEF_PROXIMITY_DISTANCE = 10; // dp

    private static final String DEF_INITIAL_STATE = "stop";
    private static final String DEF_AWAKE_STATE = "awake";
    private static final String DEF_MOVE_STATE_PREFIX = "move";
    private static final String DEF_WALL_STATE_PREFIX = "wall";

    private float acceleration;
    private float max_velocity;
    private float deacceleration_distance;
    private float proximity_distance;

    private String initial_state;
    private String awake_state;
    private String move_state_prefix;
    private String wall_state_prefix;

    private HashMap<String, Motion> motions = new HashMap<String, Motion>();

    private static class Motion
    {
        private String name;
        private String next_state = null;

        private boolean check_move = false;
        private boolean check_wall = false;

        private MotionDrawable items = null;
    }

    public MotionParams(Context context, Resources res, int resid)
    {
        XmlPullParser xml = res.getXml(resid);
        AttributeSet attrs = Xml.asAttributeSet(xml);
        try {
            parseXml(res, xml, attrs);
        }
        catch(Exception e) {
            throw new IllegalArgumentException(
                "Load failed: " + res.getResourceName(resid), e);
        }
    }

    public float getAcceleration()
    {
        return acceleration;
    }

    public float getMaxVelocity()
    {
        return max_velocity;
    }

    public float getDeaccelerationDistance()
    {
        return deacceleration_distance;
    }

    public float getProximityDistance()
    {
        return proximity_distance;
    }

    public boolean hasState(String state)
    {
        return motions.containsKey(state);
    }

    public String getInitialState()
    {
        return initial_state;
    }

    public String getAwakeState()
    {
        return awake_state;
    }

    public String getMoveState(MoveDirection dir)
    {
        return move_state_prefix +
            (dir == MoveDirection.UP ? "Up" :
             dir == MoveDirection.DOWN ? "Down" :
             dir == MoveDirection.LEFT ? "Left" :
             dir == MoveDirection.RIGHT ? "Right" :
             dir == MoveDirection.UP_LEFT ? "UpLeft" :
             dir == MoveDirection.UP_RIGHT ? "UpRight" :
             dir == MoveDirection.DOWN_LEFT ? "DownLeft" :
             dir == MoveDirection.DOWN_RIGHT ? "DownRight" :
             "");
    }

    public String getWallState(WallDirection dir)
    {
        return wall_state_prefix +
            (dir == WallDirection.UP ? "Up" :
             dir == WallDirection.DOWN ? "Down" :
             dir == WallDirection.LEFT ? "Left" :
             dir == WallDirection.RIGHT ? "Right" :
             "");
    }

    public String getNextState(String state)
    {
        Motion motion = motions.get(state);
        return (motion != null ? motion.next_state : null);
    }

    public boolean needCheckMove(String state)
    {
        Motion motion = motions.get(state);
        return (motion != null ? motion.check_move : false);
    }

    public boolean needCheckWall(String state)
    {
        Motion motion = motions.get(state);
        return (motion != null ? motion.check_wall : false);
    }

    public MotionDrawable getDrawable(String state)
    {
        Motion motion = motions.get(state);
        return (motion != null ? motion.items : null);
    }

    private void parseXml(Resources res, XmlPullParser xml, AttributeSet attrs)
        throws XmlPullParserException, IOException
    {
        int depth = xml.getDepth();
        while(true) {
            int type = xml.next();
            if(type == XmlPullParser.END_DOCUMENT ||
               (type == XmlPullParser.END_TAG && depth >= xml.getDepth())) {
                break;
            }
            if(type != XmlPullParser.START_TAG) {
                continue;
            }

            String name = xml.getName();
            if(TAG_MOTION_PARAMS.equals(name)) {
                parseMotionParams(res, xml, attrs);
            }
            else {
                throw new IllegalArgumentException("unknown tag: " + name);
            }
        }
    }

    private void parseMotionParams(Resources res,
                                   XmlPullParser xml, AttributeSet attrs)
        throws XmlPullParserException, IOException
    {
        float density = res.getDisplayMetrics().density;
        acceleration = density * attrs.getAttributeIntValue(
            null, ATTR_ACCELERATION, DEF_ACCELERATION);
        deacceleration_distance = density * attrs.getAttributeIntValue(
            null, ATTR_DEACCELERATION, DEF_DEACCELERATE_DISTANCE);
        max_velocity = density * attrs.getAttributeIntValue(
            null, ATTR_MAX_VELOCITY, DEF_MAX_VELOCITY);
        proximity_distance = density * attrs.getAttributeIntValue(
            null, ATTR_PROXIMITY, DEF_PROXIMITY_DISTANCE);

        initial_state = attrs.getAttributeValue(null, ATTR_INITIAL_STATE);
        initial_state = (initial_state != null ? initial_state :
                         DEF_INITIAL_STATE);

        awake_state = attrs.getAttributeValue(null, ATTR_AWAKE_STATE);
        awake_state = (awake_state != null ? awake_state : DEF_AWAKE_STATE);

        move_state_prefix =
            attrs.getAttributeValue(null, ATTR_MOVE_STATE_PREFIX);
        move_state_prefix = (move_state_prefix != null ? move_state_prefix :
                             DEF_MOVE_STATE_PREFIX);

        wall_state_prefix =
            attrs.getAttributeValue(null, ATTR_WALL_STATE_PREFIX);
        wall_state_prefix = (wall_state_prefix != null ? wall_state_prefix :
                             DEF_WALL_STATE_PREFIX);

        int depth = xml.getDepth();
        while(true) {
            int type = xml.next();
            if(type == XmlPullParser.END_DOCUMENT ||
               (type == XmlPullParser.END_TAG && depth >= xml.getDepth())) {
                break;
            }
            if(type != XmlPullParser.START_TAG) {
                continue;
            }

            String name = xml.getName();
            if(TAG_MOTION.equals(name)) {
                parseMotion(res, xml, attrs);
            }
            else {
                throw new IllegalArgumentException("unknown tag: " + name);
            }
        }
    }

    private void parseMotion(Resources res,
                             XmlPullParser xml, AttributeSet attrs)
        throws XmlPullParserException, IOException
    {
        Motion motion = new Motion();

        motion.name = attrs.getAttributeValue(null, ATTR_STATE);
        if(motion.name == null) {
            throw new IllegalArgumentException(
                "state is not specified: " + attrs.getPositionDescription());
        }

        int duration = attrs.getAttributeIntValue(null, ATTR_DURATION, -1);
        motion.next_state = attrs.getAttributeValue(
            null, ATTR_NEXT_STATE);
        motion.check_move = attrs.getAttributeBooleanValue(
            null, ATTR_CHECK_MOVE, false);
        motion.check_wall = attrs.getAttributeBooleanValue(
            null, ATTR_CHECK_WALL, false);

        motion.items = new MotionDrawable();

        int depth = xml.getDepth();
        while(true) {
            int type = xml.next();
            if(type == XmlPullParser.END_DOCUMENT ||
               (type == XmlPullParser.END_TAG && depth >= xml.getDepth())) {
                break;
            }
            if(type != XmlPullParser.START_TAG) {
                continue;
            }

            String name = xml.getName();
            if(TAG_ITEM.equals(name)) {
                parseItem(res, motion.items, xml, attrs);
            }
            else if(TAG_REPEAT_ITEM.equals(name)) {
                parseRepeatItem(res, motion.items, xml, attrs);
            }
            else {
                throw new IllegalArgumentException("unknown tag: " + name);
            }
        }

        motion.items.setTotalDuration(duration);
        motion.items.setRepeatCount(1);

        motions.put(motion.name, motion);
    }

    private void parseItem(Resources res, MotionDrawable items,
                           XmlPullParser xml, AttributeSet attrs)
        throws XmlPullParserException, IOException
    {
        int drawable = attrs.getAttributeResourceValue(
            null, ATTR_ITEM_DRAWABLE, 0);
        int duration = attrs.getAttributeIntValue(
            null, ATTR_ITEM_DURATION, -1);

        items.addFrame(res.getDrawable(drawable), duration);
    }

    private void parseRepeatItem(Resources res, MotionDrawable items,
                                 XmlPullParser xml, AttributeSet attrs)
        throws XmlPullParserException, IOException
    {
        int duration = attrs.getAttributeIntValue(
            null, ATTR_ITEM_DURATION, -1);
        int repeat = attrs.getAttributeIntValue(
            null, ATTR_ITEM_REPEAT_COUNT, -1);
        MotionDrawable dr = new MotionDrawable();

        int depth = xml.getDepth();
        while(true) {
            int type = xml.next();
            if(type == XmlPullParser.END_DOCUMENT ||
               (type == XmlPullParser.END_TAG && depth >= xml.getDepth())) {
                break;
            }
            if(type != XmlPullParser.START_TAG) {
                continue;
            }

            String name = xml.getName();
            if(TAG_ITEM.equals(name)) {
                parseItem(res, dr, xml, attrs);
            }
            else if(TAG_REPEAT_ITEM.equals(name)) {
                parseRepeatItem(res, dr, xml, attrs);
            }
            else {
                throw new IllegalArgumentException("unknown tag: " + name);
            }
        }

        dr.setTotalDuration(duration);
        dr.setRepeatCount(repeat);
        items.addFrame(dr, -1);
    }
}
