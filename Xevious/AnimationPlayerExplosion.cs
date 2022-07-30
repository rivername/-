using UnityEngine;

public class AnimationPlayerExplosion : MonoBehaviour
{
    private Animator anim;

    void Start()
    {
        anim = GetComponent<Animator>();
    }

    void Update()
    {
        AnimatorStateInfo info = anim.GetCurrentAnimatorStateInfo(0);
        float currentFrame = info.normalizedTime;

        if (currentFrame >= 1.0f)
        {
            Destroy(this.gameObject);
        }
    }
}
