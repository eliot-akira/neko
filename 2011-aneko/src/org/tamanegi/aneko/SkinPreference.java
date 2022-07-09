package org.tamanegi.aneko;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.content.res.TypedArray;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.View;
import android.widget.ImageView;
import android.widget.SimpleAdapter;
import android.widget.Toast;

public class SkinPreference extends DialogPreference
{
    private static final String KEY_ICON = "icon";
    private static final String KEY_LABEL = "label";
    private static final String KEY_CHECK = "check";
    private static final String KEY_COMPONENT = "component";

    private static final String[] ITEM_FROM = {
        KEY_ICON, KEY_LABEL, KEY_CHECK
    };
    private static final int[] ITEM_TO = {
        R.id.item_icon, R.id.item_label, R.id.item_check
    };

    private List<Map<String, Object>> data;
    private int clicked_index;

    private ComponentName val;

    public SkinPreference(Context context)
    {
        this(context, null);
    }

    public SkinPreference(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public void setValue(String str)
    {
        this.val = ComponentName.unflattenFromString(str);
        persistString(str);
    }

    public void setValue(ComponentName val)
    {
        setValue(val != null ? val.flattenToString() : "");
    }

    public ComponentName getValue()
    {
        return val;
    }

    protected Object onGetDefaultValue(TypedArray a, int index)
    {
        return a.getString(index);
    }

    @Override
    protected void onSetInitialValue(boolean restore, Object defaultValue)
    {
        setValue(restore ? getPersistedString("") : (String)defaultValue);
    }

    @Override
    protected void onPrepareDialogBuilder(AlertDialog.Builder builder)
    {
        super.onPrepareDialogBuilder(builder);

        data = createListData();
        clicked_index = -1;

        SimpleAdapter adapter = new SimpleAdapter(
            getContext(), data, R.layout.preference_skin_item,
            ITEM_FROM, ITEM_TO);
        adapter.setViewBinder(new IconViewBinder());

        builder
            .setAdapter(
                adapter,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int witch) {
                        clicked_index = witch;

                        SkinPreference.this.onClick(
                            dialog, DialogInterface.BUTTON_POSITIVE);
                        dialog.dismiss();
                    }
                })
            .setPositiveButton(null, null)
            .setNeutralButton(
                R.string.pref_skin_search_button,
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int witch) {
                        Intent intent = new Intent(Intent.ACTION_VIEW);
                        intent.setData(
                            Uri.parse(getContext().getString(
                                          R.string.skin_search_uri)));
                        try {
                            getContext().startActivity(intent);
                        }
                        catch(ActivityNotFoundException e) {
                            Toast.makeText(getContext(),
                                           R.string.msg_market_not_found,
                                           Toast.LENGTH_SHORT)
                                .show();
                        }

                        dialog.dismiss();
                    }
                });
    }

    @Override
    protected void onDialogClosed(boolean positiveResult)
    {
        super.onDialogClosed(positiveResult);

        if(positiveResult && clicked_index >= 0) {
            ComponentName comp =
                (ComponentName)data.get(clicked_index).get(KEY_COMPONENT);
            if(callChangeListener(comp)) {
                setValue(comp);
            }
        }
    }

    private List<Map<String, Object>> createListData()
    {
        Context context = getContext();
        PackageManager pm = context.getPackageManager();

        Intent[] internals = {
            new Intent(context, NekoSkin.class),
        };
        Intent intent = new Intent(AnimationService.ACTION_GET_SKIN);
        List<ResolveInfo> activities = pm.queryIntentActivityOptions(
            null, internals, intent, 0);

        List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();

        for(ResolveInfo info : activities) {
            ComponentName comp = new ComponentName(
                info.activityInfo.packageName, info.activityInfo.name);

            Map<String, Object> data = new HashMap<String, Object>();
            data.put(KEY_ICON, info.loadIcon(pm));
            data.put(KEY_LABEL, info.loadLabel(pm));
            data.put(KEY_CHECK, Boolean.valueOf(comp.equals(val)));
            data.put(KEY_COMPONENT, comp);

            list.add(data);
        }

        return list;
    }

    private class IconViewBinder implements SimpleAdapter.ViewBinder
    {
        @Override
        public boolean setViewValue(View view, Object data, String text)
        {
            if(view.getId() != R.id.item_icon) {
                return false;
            }

            ((ImageView)view).setImageDrawable((Drawable)data);
            return true;
        }
    }
}
