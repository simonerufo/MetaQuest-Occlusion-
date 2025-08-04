/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * Licensed under the Oculus SDK License Agreement (the "License");
 * you may not use the Oculus SDK except in compliance with the License,
 * which is provided at the time of installation or download, or which
 * otherwise accompanies this software in either electronic or hard copy form.
 *
 * You may obtain a copy of the License at
 * https://developer.oculus.com/licenses/oculussdk/
 *
 * Unless required by applicable law or agreed to in writing, the Oculus SDK
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.oculus.xrpassthroughocclusion;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import java.util.HashSet;
import java.util.Set;

public class MainActivity extends android.app.Activity {
  public static final String APP_NAME = "XrPassthroughOcclusion";

  /// Tag used for log messages.
  public static final String TAG = APP_NAME;

  // Just adding these permissions to manifest does not work for Android >= 11. Requests have to
  // be made explicitly in the code!
  private static final String PERMISSIONS[] = {"com.oculus.permission.USE_SCENE"};

  private final int PERMISSION_REQUEST_CODE = 1;

  private Set<String> mRequestedPermissions = new HashSet<>();

  private Set<String> getMissingPermissions() {
    Set<String> missingPermissions = new HashSet<>();
    for (String permission : PERMISSIONS) {
      Log.d(TAG, String.format("Checking permission %s", permission));
      if (checkSelfPermission(permission) != PackageManager.PERMISSION_GRANTED) {
        Log.d(TAG, String.format("Requesting permission %s", permission));
        missingPermissions.add(permission);
      }
    }
    return missingPermissions;
  }

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    // startNativeActivity() is called if/when all permissions are granted, otherwise app is
    // terminated
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
      Set<String> missingPermissions = getMissingPermissions();
      if (missingPermissions.isEmpty()) {
        startNativeActivity();
      } else {
        mRequestedPermissions = missingPermissions;
        requestPermissions(
            mRequestedPermissions.toArray(new String[missingPermissions.size()]),
            PERMISSION_REQUEST_CODE);
      }
    }
  }

  @Override
  public void onDestroy() {
    Log.d(TAG, String.format("onDestroy() called"));
    super.onDestroy();
  }

  private boolean allPermissionsGranted() {
    return mRequestedPermissions.isEmpty();
  }

  @Override
  public void onRequestPermissionsResult(
      int requestCode, String permissions[], int[] grantResults) {
    Log.d(TAG, "onRequestPermissionsResult() called");
    final int length = permissions.length;
    if (requestCode == PERMISSION_REQUEST_CODE && length > 0) {
      for (int i = 0; i < length; ++i) {
        String permission = permissions[i];
        if (grantResults[i] == PackageManager.PERMISSION_GRANTED) {
          Log.d(TAG, String.format("Permission %s granted", permission));
          mRequestedPermissions.remove(permission);
        } else {
          Log.d(TAG, String.format("Permission %s DENIED", permission));
          finish();
          return;
        }
      }
      if (allPermissionsGranted()) {
        startNativeActivity();
      }
    }
  }

  private void startNativeActivity() {
    Log.d(TAG, "Starting native activity");

    Intent intent = new Intent(getApplicationContext(), MainNativeActivity.class);
    startActivity(intent);
    finish();
  }
}
